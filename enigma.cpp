/******************************************************************************

Name:        Colton Aylix

Purpose:

    This program is a simulation of a simplified version of the German world
    war two encryption machine named Enigma.

Details:
    The simulation is simplified in that it only uses two rotors instead of the
    minimum of three. The initial translation settings for both rotors and the
    reflector are read from a file.

    The program prompts for the names of five files for:
         - the two rotors
         - a reflector
         - an input data file
         - an output result file

    This program preserves a very important property of the Enigma machine:
    decryption is done by running the enigma with the encoded message.

Input files:

    The rotor and reflector files specify a permutation of the 26 lower case
    letters by giving the result of applying the permutation to the 26
    letters listed in alphabetical order.

    A valid reflector must be be composed of thirteen 2-cycles,
          i.e. letter1 -> letter2 and letter2 -> letter1

    The input data file contains a "message"  to be encrypted or decrypted.
    It must consist entirely of lower case alphabetic characters, spaces, and
    newlines.

Output:

   The output result file contains the message resulting from the encrypted or
   decrypted of the input message.

******************************************************************************/

#include <iostream>
#include <fstream>
using namespace std;

const int ARRAY_SIZE = 26;
const int FILENAME_LEN = 80;
const char LITTLE_A = 'a';
const char LITTLE_Z = 'z';

bool loadArray(int array[], const char filename[]);
bool loadRotor(int rotor[], const char filename[]);
bool loadReflector(int reflect[], const char filename[]);
void showTranslation(const int array[]);
void rotateRotor(int rotor[]);
char intToChar(int value);
int charToInt(char letter);
char translateLetter(int index, int shift);
char indexToLetter(int index);
char lookupForward(char ch, int rotor[]);
char lookupBackward(char ch, int rotor[]);

int main()
{
    int rotorOne[ARRAY_SIZE]; // the loadable components of the Enigma
    int rotorTwo[ARRAY_SIZE];
    int reflector[ARRAY_SIZE];
    int counter = 0;

    char ch; // the character to be encrypted

    char rotorOneFileName[FILENAME_LEN];
    char rotorTwoFileName[FILENAME_LEN];
    char reflectorFileName[FILENAME_LEN];
    char plainFileName[FILENAME_LEN];
    char cypherFileName[FILENAME_LEN];

    ifstream infile;  // for reading the input text
    ofstream outfile; // for writing the output text

    cout << "Enter the file name for the 1st rotor: ";
    cin >> rotorOneFileName;
    cout << "Enter the file name for the 2nd rotor: ";
    cin >> rotorTwoFileName;
    cout << "Enter the file name for the reflector: ";
    cin >> reflectorFileName;
    cout << "Enter the file name for the plain text: ";
    cin >> plainFileName;
    cout << "Enter the file name for the cypher text: ";
    cin >> cypherFileName;

    infile.open(plainFileName);

    if (infile.fail())
    {
        cout << "Could not open file: " << plainFileName
             << " for input." << endl;
        cout << "Exiting program." << endl;
    }
    else
    {
        outfile.open(cypherFileName);

        if (outfile.fail())
        {
            cout << "Could not open file: " << cypherFileName
                 << " for output." << endl;
            cout << "Exiting program." << endl;
            infile.close();
        }
        else
        {
            if (!loadRotor(rotorOne, rotorOneFileName))
            {
                cout << "Problem with " << rotorOneFileName << endl;
                cout << "Exiting program." << endl;
                infile.close();
                outfile.close();
            }
            else
            {
                if (!loadRotor(rotorTwo, rotorTwoFileName))
                {
                    cout << "Problem with " << rotorTwoFileName << endl;
                    cout << "Exiting program." << endl;
                    infile.close();
                    outfile.close();
                }
                else
                {
                    if (!loadReflector(reflector, reflectorFileName))
                    {
                        cout << "Problem with " << reflectorFileName << endl;
                        cout << "Exiting program." << endl;
                        infile.close();
                        outfile.close();
                    }
                    else
                    {
                        infile.get(ch);

                        while (!infile.eof())
                        {
                            if (ch != ' ' && ch != '\n')
                            {
                                ch = lookupForward(ch, rotorOne);
                                ch = lookupForward(ch, rotorTwo);
                                ch = lookupForward(ch, reflector);
                                ch = lookupBackward(ch, rotorTwo);
                                ch = lookupBackward(ch, rotorOne);

                                rotateRotor(rotorOne);
                                counter++;

                                if (counter % 26 == 0)
                                    rotateRotor(rotorTwo);
                            }

                            outfile.put(ch);
                            infile.get(ch);
                        }

                        infile.close();
                        outfile.close();

                        cout << "Encryption successfully completed." << endl;
                    }
                }
            }
        }
    }

    return 0;
}

/******************************************************************************

Name:     showTranslation

Purpose:  Displays to the screen the contents of an array of 26 integers but by,
          showing:
            - the array indices as if they are characters starting at 'a'
            - the array element contents as a translation character, meaning
              if the index character is input then it would convert to the
              translation character, ex. if the contents of array element 0
              were 23 then the output would be:
                     a ...
                     | |||
                     x ...

Input Parameters:  An array of exactly 26 characters.

******************************************************************************/
void showTranslation(const int array[])
{
    char ch;   // the current character to write
    int index; // index into the array

    for (ch = LITTLE_A; ch <= LITTLE_Z; ch = intToChar(charToInt(ch) + 1))
        cout << ch << " ";

    cout << endl;

    for (ch = LITTLE_A; ch <= LITTLE_Z; ch = intToChar(charToInt(ch) + 1))
        cout << "| ";

    cout << endl;

    index = 0;

    for (index = 0; index < ARRAY_SIZE; index++)
    {
        cout << translateLetter(index, array[index]) << " ";
    }

    cout << endl;

    return;
}

char intToChar(int value)
{
    char letter;

    letter = static_cast<char>(value);

    return letter;
}

int charToInt(char letter)
{
    int number;

    number = static_cast<int>(letter);

    return number;
}

/******************************************************************************

Name:     translateLetter

Purpose:  Given a starting letter in the form of an index and an integer
          difference, i.e. a shift, computes the translation character resulting
          from this combination.

Details:  The starting letter is generated by adding the index with the
          character 'a'.
          The translation character is index + 'a' + shift

Input Parameters:
          An array index between 0 and 25, which represents a letter.
          An integer difference, the shift between the starting letter, the
          index, and the translation character.

Returns;  The translation character, a lower case letter between 'a' and 'z'

******************************************************************************/
char translateLetter(int index, int shift)
{
    char ch;

    ch = intToChar(index + charToInt(LITTLE_A) + shift);

    return ch;
}

/******************************************************************************

Name:     indexToLetter

Purpose:  Translates a numeric array index into the corresponding letter that
          this index represents.

Details:  C++ only allows numeric array indices, so this function converts
          a numeric index into an alphabetic index. This is done by
          starting letter is generated by adding the character 'a' to
          the numeric array index.
          'a' + 0 = 'a'
          'a' + 1 = 'b'

          This assumes that ASCII character encoding is being used.
          This works because ASCII characters are consecutive, in
           alphabetic order starting with the 'a'

Input Parameters:
          An array index between 0 and 25.

Returns:  The corresponding lower case letter.

******************************************************************************/
char indexToLetter(int index)
{
    char ch;

    ch = intToChar(index + charToInt(LITTLE_A));

    return ch;
}

/******************************************************************************

Name:     loadArray

Purpose:  Populates an array with integer differences between two alphabetic
          letters.

Details:  The two letters are a starting letter and a translation letter.
          The starting letter is current index into the array. The translation
          letter comes from a file, which must contain at least 26
          non-whitespace characters.
          The integer difference is the translation letter - the starting letter

Input Parameters:  The name of the input file from which to read characters.

Output Parameters: An array populated with the contents of the file.

Returns:  The status of the (attempted) file opening.

******************************************************************************/
bool loadArray(int array[], const char filename[])
{
    ifstream fin;
    char ch;
    int index;

    fin.open(filename);

    if (!fin.fail())
    {
        for (index = 0; index < ARRAY_SIZE; index++)
        {
            fin >> ch;
            array[index] = charToInt(ch) - (charToInt(LITTLE_A) + index);
        }

        fin.close();
    }
    return !fin.fail();
}

/******************************************************************************

Name:     loadReflector

Purpose:  Populates and validates a reflector array with data from a file.

Details:  The file must contain at least 26 non-whitespace characters.
          A reflector is valid:
           -  if for each plain text letter translates to a cipher letter and
              the same cipher letter translates to the original plain text
              letter.
          AND
           - no letter translates to itself.
          The function returns the success of both actions: the file existing
          and meeting the requirements of a reflector.

Input Parameters:  The name of the file from which to read.

Output Parameters: The reflector array, populated with numeric difference
                   between each input letter and the translation letter
                   from the file.

Returns:  The status of the (attempted) load operation.

******************************************************************************/
bool loadReflector(int reflector[], const char filename[])
{
    bool OK;     // the status of the load operation
    int index;   // index of the current plain text letter being checked
    int t_index; // index corresponding cipher text letter

    OK = loadArray(reflector, filename);

    if (OK)
    {
        index = 0;
        while (index < ARRAY_SIZE && OK)
        {
            t_index = reflector[index] + index;

            if (translateLetter(t_index, reflector[t_index]) !=
                    indexToLetter(index) ||
                (reflector[index] == 0))
                OK = false;
            index++;
        }
    }

    return OK;
}

/******************************************************************************

Name:     loadRotor

Purpose:  Populates and validates a rotor array with data from a file.
          The file must contain at least 26 non-whitespace characters.
          < insert rotor validation rules >
          The function returns the success of both actions: the file existing
          and meeting the requirements of a rotor.

Input Parameters:  The name of the file from which to read.

Output Parameters: A rotor populated with the numeric difference between
                   each input letter and the translation letter, from the
                   file.

Returns:  The status of the (attempted) load operation.

Method: <complete>

******************************************************************************/
bool loadRotor(int rotor[], const char filename[])
{
    bool OK; // the status of the load operation
    char ch;
    int letterCount[ARRAY_SIZE] = {};
    int index;
    int countIndex;

    OK = loadArray(rotor, filename);

    if (OK)
    {
        for (countIndex = 0; countIndex < ARRAY_SIZE; countIndex++)
        {
            ch = translateLetter(countIndex, rotor[countIndex]);

            if (ch >= 'a' && ch <= 'z')
            {
                index = charToInt(ch) - charToInt('a');

                letterCount[index]++;
            }
        }
        for (countIndex = 0; countIndex < ARRAY_SIZE; countIndex++)
        {
            if (letterCount[countIndex] != 1)
            {
                OK = false;
            }
        }
    }
    return OK;
}

/******************************************************************************

Name:     rotateRotor

Purpose:  Rotates the the values inside the rotor array to the left one. It
          also makes sure the values inside the array do not go out of the
          bounds of the array 0 - 25.
Details:  Takes in a loaded array and shifts every value to the index lower
          than its current position. If the index + the value of that index
          is less than zero, that value gets 26 added to it. If the index +
          the value in that index is greater than 25, that value gets 26
          subtracted from it. This keeps the values in bounds.

Input Parameters:  A loaded array representing a rotor.

******************************************************************************/
void rotateRotor(int rotor[])
{

    int temp = rotor[0];

    for (int index = 0; index < ARRAY_SIZE; index++)
    {
        rotor[index] = rotor[index + 1];

        if ((index + 1) > 25)
            rotor[index] = temp;

        if (index + rotor[index] < 0)
            rotor[index] = rotor[index] + 26;
        else if (index + rotor[index] > 25)
            rotor[index] = rotor[index] - 26;
    }
}

/******************************************************************************

Name:     lookupForward

Purpose:  Takes a character and determines the index based on its value. The
          index is then added to the value inside that index to map it to the
          corresponding character.

Input Parameters:  A character and a loaded rotor.

Returns:  A new character.

******************************************************************************/
char lookupForward(char ch, int rotor[])
{
    int index;
    int num; // a number representing the transformed letter
    char newChar;

    index = charToInt(ch) - charToInt('a');

    num = (index + rotor[index]) + charToInt('a');

    newChar = intToChar(num);

    return newChar;
}

/******************************************************************************

Name:     lookupBackward

Purpose:  Does the same as lookupForward but does it in reverse.

Details:  Takes a character and loacates the intital value entered into
          lookupForward by scanning the array for the same character. If it
          is found the corresponding character is returned.

Input Parameters:  A character and a loaded array.

Returns:  A character.

******************************************************************************/
char lookupBackward(char ch, int rotor[])
{
    char tempChar;
    char translation;

    for (int tempNum = charToInt('a'); tempNum <= charToInt('z'); tempNum++)
    {
        tempChar = intToChar(tempNum);

        if (lookupForward(tempChar, rotor) == ch)
        {
            translation = tempChar;
        }
    }
    return translation;
}