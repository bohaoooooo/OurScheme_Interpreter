# include <stdio.h>
# include <iostream>
# include <string>
# include <string.h>
# include <vector>
# include <exception>
# include <sstream> 
# include <iomanip>

using namespace std ;

int gTestNum = 0 ;
bool gexitMsg = false ;
bool geof = false ;
int gerrorMsg = 0 ;
int gerrorEvalMsg = 0 ;
int gLine = 1 ; // because first char is '\n'
int gColumn = 0 ;
bool gDontChange = false ;
int gLevel = 0 ;

enum TokenType {
  LEFTPAREN,      // '('
  RIGHTPAREN,     // ')'
  INT,            // e.g., '123', '+123', '-123'
  STRING,         // "string's (example)." (strings do not extend across lines)
  DOT,            // '.'
  FLOAT,          // '123.567', '123.', '.567', '+123.4', '-.123'
  NIL,            // 'nil' or '#f', but not 'NIL' nor 'nIL'
  T,              // 't' or '#t', but not 'T' nor '#T'
  QUOTE,          // '
  SYMBOL
} ; // TokenType

struct Token {
  string content ;
  TokenType type ;
  int mLine ;
  int mColumn ;
} ; // Token

struct TreeNode {
  Token aToken ;
  bool isEmpty ; // initial is false
  TreeNode *left ;
  TreeNode *right ;
} ; // TreeNode

typedef TreeNode * TreeNodePtr ;

vector<TreeNodePtr> gVariable ;


// ---------------------------- Scanner and Parser's Function  ----------------------------

char GetNextChar() ;
char PeekNextChar() ;
void GetToken( Token &nextToken ) ;
bool IsWhiteSpace( char ch ) ;
bool IsDigit( char ch ) ;
bool IsLetter( char ch ) ;
float StringToFloat( string inputString ) ;
int StringToInt( string inputString ) ;
void SkipComment() ;
void SkipErrorLine() ;
char SkipWhiteSpace() ;
bool IsAtom( Token aToken ) ;
void ReadSExp( Token aToken, vector<Token> &sExpList ) ;
void GetSExpForTree( vector<Token> sExpList, int &i, vector<Token> &sExpForTree ) ;
TreeNodePtr BuildTree( vector<Token> sExpList, int &i ) ;


// ---------------------------- Eval's Function  ----------------------------

TreeNodePtr EvalSExp( TreeNodePtr inSExpTree ) ;
bool CheckBounded( string variable, int &index ) ;
bool IsSameTree( TreeNodePtr tree1, TreeNodePtr tree2 ) ;
void EvalDefine( TreeNodePtr inSExpTree ) ;
TreeNodePtr EvalQuote( TreeNodePtr inSExpTree ) ;
TreeNodePtr EvalCons( TreeNodePtr inSExpTree ) ;
TreeNodePtr EvalList( TreeNodePtr inSExpTree ) ;
TreeNodePtr EvalCar( TreeNodePtr inSExpTree ) ;
TreeNodePtr EvalCdr( TreeNodePtr inSExpTree ) ;
TreeNodePtr EvalPredicates( TreeNodePtr inSExpTree, string evalType ) ;
TreeNodePtr EvalArithmetic( TreeNodePtr inSExpTree, string evalType ) ;
TreeNodePtr EvalNot( TreeNodePtr inSExpTree ) ;
TreeNodePtr EvalAnd( TreeNodePtr inSExpTree ) ;
TreeNodePtr EvalOr( TreeNodePtr inSExpTree ) ;
TreeNodePtr EvalBiggerSmaller( TreeNodePtr inSExpTree, string evalType ) ;
TreeNodePtr EvalStringOpr( TreeNodePtr inSExpTree, string evalType ) ;
TreeNodePtr EvalEq( TreeNodePtr inSExpTree, string evalType ) ;
TreeNodePtr EvalBegin( TreeNodePtr inSExpTree ) ;
TreeNodePtr EvalIf( TreeNodePtr inSExpTree ) ;
TreeNodePtr EvalCond( TreeNodePtr inSExpTree ) ;
TreeNodePtr EvalClean( TreeNodePtr inSExpTree ) ;
TreeNodePtr EvalExit( TreeNodePtr inSExpTree ) ;
void PrintTree( TreeNodePtr sExpTree, TreeNodePtr parentNode, vector<Token> &printList ) ;
void PrintRightTree( TreeNodePtr sExpTree, vector<Token> &printList ) ;
void PrettyPrint( vector<Token> printList ) ;
void PrettyPrint2( vector<Token> printList ) ;
string IntToString( int inputInt ) ;
string FloatToString( float inputFloat ) ;


// ---------------------------- Main Function  ----------------------------

int main() {
  scanf( "%d", &gTestNum ) ;
  
  char temp = '\0' ;
  scanf( "%c", &temp ) ;

  cout << "Welcome to OurScheme!" << endl ;
  cout << endl << "> ";
  
  // END-OF-FILE NOT encountered and NOT 「user entered '(exit)'」
  while ( gexitMsg == false && geof == false ) { 
    Token aInputToken ;
    vector<Token> syntaxToken ;
    vector<Token> syntaxForTree ;
    vector<Token> printList ;

    int i = 0 ;
    bool hasCoutArr = false ;

    gerrorMsg = 0 ;

    if ( geof == false ) {
      try {
        ReadSExp( aInputToken, syntaxToken ) ;

        if ( gexitMsg == false && geof == false && syntaxToken.size() > 0 ) {
          GetSExpForTree( syntaxToken, i, syntaxForTree ) ;

          i = 0 ;
          TreeNodePtr sExpTree = new TreeNode ;
          sExpTree = BuildTree( syntaxForTree, i ) ;

          try {
            TreeNodePtr resultTree = new TreeNode ;
            resultTree = EvalSExp( sExpTree ) ;
            
            if ( resultTree != NULL ) {
              PrintTree( resultTree, NULL, printList ) ;
              PrettyPrint( printList ) ;
            } // if()
            
          } // try
          catch( vector<Token> printList ) {
            if ( gerrorEvalMsg == 1 ) {
              cout << "ERROR (non-list) : " ;
              PrettyPrint2( printList ) ;
            } // if()
            else if ( gerrorEvalMsg == 2 ) {
              cout << "ERROR (attempt to apply non-function) : " ;
              PrettyPrint( printList ) ;
            } // else if()
            else if ( gerrorEvalMsg == 3 ) {
              cout << "ERROR (DEFINE format) : " ;
              PrettyPrint2( printList ) ;
            } // else if()
            else if ( gerrorEvalMsg == 4 ) {
              cout << "ERROR (unbound symbol) : " ;
              PrettyPrint2( printList ) ;
            } // else if()
            // else if ( gerrorEvalMsg == 5 ) is already cout before
            else if ( gerrorEvalMsg == 6 ) {
              cout << " with incorrect argument type) : " ;
              PrettyPrint2( printList ) ;
            } // else if()
            else if ( gerrorEvalMsg == 7 ) {
              cout << "ERROR (division by zero) : " ;
              PrettyPrint2( printList ) ;
            } // else if()
            else if ( gerrorEvalMsg == 8 ) {
              cout << "ERROR (no return value) : " ;
              PrettyPrint2( printList ) ;
            } // else if()
            else if ( gerrorEvalMsg == 9 ) {
              cout << "ERROR (COND format) : " ;
              PrettyPrint2( printList ) ;
            } // else if( gerrorEvalMsg == 10 || gerrorEvalMsg == 11 ) 
            // else if 

            if ( gexitMsg == false && geof == false ) {
              cout << endl << "> " ;
              hasCoutArr = true ;
            } // if()

          } // catch
        } // if()
      } // try
      catch( Token aToken ) {
        if ( gerrorMsg == 1 && geof == false ) {
          cout << "ERROR (unexpected token) : atom or '(' expected when token at Line " ;
          cout << aToken.mLine <<  " Column " << aToken.mColumn ;
          cout << " is >>" << aToken.content << "<<" << endl ;
        } // if()
        else if ( gerrorMsg == 2 && geof == false ) {
          cout << "ERROR (unexpected token) : ')' expected when token at Line " ;
          cout << aToken.mLine <<  " Column " << aToken.mColumn ;
          cout << " is >>" << aToken.content << "<<" << endl ;
        } // else if()
        else if ( gerrorMsg == 3 && geof == false ) {
          cout << "ERROR (no closing quote) : END-OF-LINE encountered at Line " ;
          cout << gLine <<  " Column " << gColumn + 1 << endl ;
        } // else if()

        SkipErrorLine() ;

        if ( gexitMsg == false && geof == false ) {
          cout << endl << "> " ;
          hasCoutArr = true ;
        } // if()

      } // catch
    } // if()

    gColumn = 0 ;

    if ( gexitMsg == false && geof == false && syntaxToken.size() > 0 && hasCoutArr == false ) {
      cout << endl << "> ";
    } // if()

    // check sExp then all white-space '\n'
    char peekCh = cin.peek() ;
    while ( peekCh == ' ' ) {
      temp = cin.get() ;
      gColumn = gColumn + 1 ;

      peekCh = cin.peek() ;
    } // while()

    if ( peekCh == '\n' ) {
      temp = cin.get() ;
      gColumn = 0 ;
    } // if()
    else if ( peekCh == ';' ) {
      temp = cin.get() ;
      SkipComment() ;
    } // else if()
    

    gerrorMsg = 0 ;
    gerrorEvalMsg = 0 ;
    gLine = 1 ;
    syntaxToken.clear() ;
    syntaxForTree.clear() ;
    printList.clear() ;
    i = 0 ;
    gDontChange = false ;
    gLevel = 0 ;

  } // while()

  if ( gexitMsg == false && geof == true ) // END-OF-FILE encountered and NOT 「user entered '(exit)'」
    cout << "ERROR (no more input) : END-OF-FILE encountered" ;

  cout << endl << "Thanks for using OurScheme!" ;
} // main()


// ----------------------------  Scanner and Parser's Functions' Body  ----------------------------

char GetNextChar() {
  char ch = '\0' ;

  ch = cin.get() ;
  if ( ch != EOF ) {
    gColumn = gColumn + 1 ;
    
    if ( ch == '\n' ) {
      gLine = gLine + 1 ;
      gColumn = 0 ;
    } // if()
    
  } // if()
  else {
    geof = true ;
  } // else 

  return ch ;
} // GetNextChar()

char SkipWhiteSpace() {
  char ch = '\0' ;

  ch = cin.get() ;
  
  if ( ch != EOF ) 
    gColumn = gColumn + 1 ;

  while ( ch != EOF && IsWhiteSpace( ch ) == true ) {
    
    if ( ch == '\n' ) {
      gLine = gLine + 1 ;
      gColumn = 0 ;
    } // if()
    
    ch = cin.get() ;

    if ( ch != EOF )
      gColumn = gColumn + 1 ;
  } // while()

  if ( ch == EOF )
    geof = true ;

  return ch ;
} // SkipWhiteSpace()

char PeekNextChar() {
  char ch = '\0' ;

  ch = cin.peek() ;

  if ( ch == EOF )
    geof = true ;

  return ch ;
} // PeekNextChar()

bool IsWhiteSpace( char ch ) {
  if ( ch == ' ' || ch == '\n' || ch == '\t' ) 
    return true ;
  else
    return false ;
} // IsWhiteSpace()

bool IsLetter( char ch ) {
  if ( ch >= 'a' && ch <= 'z' )
    return true ;
  else if ( ch >= 'A' && ch <= 'Z' )
    return true ;
  else
    return false ; 
} // IsLetter()

bool IsDigit( char ch ) {
  if ( ch >= '0' && ch <= '9' )
    return true ;
  else 
    return false ;
} // IsDigit()

float StringToFloat( string inputString ) {
  float output = 0.0 ;
  bool hasDigit = false ;
  bool isNeg = false ;
  int numDigit = 0 ;
  int size = inputString.size() ;

  for ( int i = 0 ; i < size && numDigit < 3 ; i ++ ) {
    if ( inputString[i] == '.' )
      hasDigit = true ;
    else if ( inputString[i] == '-' )
      isNeg = true ;
    else if ( inputString[i] == '+' )
      isNeg = false ;
    else {
      if ( hasDigit == false )
        output = output * 10 + ( int ) inputString[i] - 48 ;
      else {
        if ( numDigit == 0 )
          output = output + ( ( int ) inputString[i] - 48 ) * 0.1 ;
        else if ( numDigit == 1 )
          output = output + ( ( int ) inputString[i] - 48 ) * 0.01 ;
        else if ( numDigit == 2 ) {
          if ( ( i + 1 ) != size && ( ( ( int ) inputString[i+1] - 48 ) >= 5 ) ) 
            output = output + ( ( int ) inputString[i] - 48 + 1 ) * 0.001 ;
          else
            output = output + ( ( int ) inputString[i] - 48 ) * 0.001 ;
        } // else if()
      
        numDigit = numDigit + 1 ;
      } // else

    } // else
  } // for()

  if ( isNeg == true )
    output = - ( output ) ;

  return output ;
} // StringToFloat()

int StringToInt( string inputString ) {
  int output = 0 ;
  bool isNeg = false ;
  int size = inputString.size() ;

  for ( int i = 0 ; i < size ; i ++ ) {
    if ( inputString[i] == '-' )
      isNeg = true ;
    else if ( inputString[i] == '+' )
      isNeg = false ;
    else
      output = output * 10 + ( int ) inputString[i] - 48 ;
  } // for()

  if ( isNeg == true )
    output = - ( output ) ;

  return output ;
} // StringToInt()

void SkipComment() {
  char nextChar = '\0' ;

  nextChar = GetNextChar() ;
  while ( nextChar != '\n' && geof != true ) {
    nextChar = GetNextChar() ;

    if ( nextChar == EOF )
      geof = true ;

  } // while()

} // SkipComment()

void SkipErrorLine() {
  char nextChar = '\0' ;

  nextChar = cin.get() ;
  while ( nextChar != '\n' && nextChar != EOF ) {
    nextChar = cin.get() ;

  } // while()

} // SkipErrorLine()

void GetToken( Token &nextToken ) {
  char nextChar = '\0' ;
  char peekChar = '\0' ;
  string tempString ;
  int tempLine = -1 ;
  int tempColumn = -1 ;

  peekChar = PeekNextChar() ;

  if ( peekChar == EOF )
    geof = true ;
  
  nextChar = SkipWhiteSpace() ;
  // LP or NIL
  if ( nextChar == '(' ) { 
    tempLine = gLine ;
    tempColumn = gColumn ;

    tempString.push_back( nextChar ) ;
    peekChar = PeekNextChar() ;

    // is () or not
    while ( IsWhiteSpace( peekChar ) == true ) {
      nextChar = GetNextChar() ;
      peekChar = PeekNextChar() ;
    } // while()
      
    
    if ( peekChar == ')' ) {
      nextChar = GetNextChar() ;
      tempString.push_back( nextChar ) ;

      nextToken.content = "nil" ;
      nextToken.type = NIL ;
      nextToken.mLine = tempLine ;
      nextToken.mColumn = tempColumn ;
    } // if()
    else {
      nextToken.content = tempString ;
      nextToken.type = LEFTPAREN ;
      nextToken.mLine = tempLine ;
      nextToken.mColumn = tempColumn ;
    } // else
  } // if()
  // RP
  else if ( nextChar == ')' ) {
    tempString.push_back( nextChar ) ;
    nextToken.mLine = gLine ;
    nextToken.type = RIGHTPAREN ;
    nextToken.mColumn = gColumn ;
    nextToken.content = tempString ;
  } // else if()
  // INT or FLOAT or SYMBOL
  else if ( IsDigit( nextChar ) == true ) {
    tempLine = gLine ;
    tempColumn = gColumn ;

    int dotNum = 0 ;
    bool hasLetter = false ;

    tempString.push_back( nextChar ) ;
    peekChar = PeekNextChar() ;

    // read nextChar until white-space or ( or )
    while ( IsWhiteSpace( peekChar ) == false && peekChar != '(' && peekChar != ')'
            && peekChar != ';' && peekChar != '\"' && peekChar != '\'' && peekChar != EOF ) {
      nextChar = GetNextChar() ;
      tempString.push_back( nextChar ) ;

      if ( nextChar == '.' )
        dotNum = dotNum + 1 ;
      
      if ( IsLetter( nextChar ) == true )
        hasLetter = true ;

      peekChar = PeekNextChar() ;
    } // while()

    if ( hasLetter == true ) {
      nextToken.content = tempString ;
      nextToken.type = SYMBOL ;
      nextToken.mLine = tempLine ;
      nextToken.mColumn = tempColumn ;
    } // if()
    else if ( dotNum == 1 ) {
      nextToken.content = tempString ;
      nextToken.type = FLOAT ;
      nextToken.mLine = tempLine ;
      nextToken.mColumn = tempColumn ;
    } // else if()
    else if ( dotNum > 1 ) {
      nextToken.content = tempString ;
      nextToken.type = SYMBOL ;
      nextToken.mLine = tempLine ;
      nextToken.mColumn = tempColumn ;
    } // else if()
    else {
      nextToken.content = tempString ;
      nextToken.type = INT ;
      nextToken.mLine = tempLine ;
      nextToken.mColumn = tempColumn ;
    } // else

  } // else if()
  // STRING with ""
  else if ( nextChar == '\"' ) {
    tempLine = gLine ;
    tempColumn = gColumn ;

    tempString.push_back( nextChar ) ;
    peekChar = PeekNextChar() ;
    bool another = false ;

    while ( peekChar != '\"' && peekChar != '\n' ) {
      // check escape char
      if ( peekChar == '\\' ) {
        nextChar = GetNextChar() ;
        peekChar = PeekNextChar() ;
        
        // is escape char
        if ( peekChar == 'n' || peekChar == '\"' || peekChar == 't' || peekChar == '\\' ) {
          nextChar = GetNextChar() ;
          if ( nextChar == 'n' )
            nextChar = '\n' ;
          else if ( nextChar == 't' )
            nextChar = '\t' ;
          
          tempString.push_back( nextChar ) ;
        } // if()
        else {
          tempString.push_back( nextChar ) ;
        } // else
      } // if()
      else {
        nextChar = GetNextChar() ;
        tempString.push_back( nextChar ) ;
      } // else
      
      peekChar = PeekNextChar() ;
    } // while()

    if ( peekChar == '\"' ) {
      another = true ;
      nextChar = GetNextChar() ;
      tempString.push_back( nextChar ) ;
    } // if()

    if ( another == true ) {
      nextToken.content = tempString ;
      nextToken.type = STRING ;
      nextToken.mLine = tempLine ;
      nextToken.mColumn = tempColumn ;
    } // if()
    else { // extend across lines
      tempString.clear() ;
      gerrorMsg = 3 ;
      throw ( nextToken ) ;
    } // else
  } // else if()
  // DOT or FLOAT or SYMBOL
  else if ( nextChar == '.' ) {
    int dotNum = 1 ;
    int posOrNegNum = 0 ;
    bool hasLetter = false ;
    bool aloneDot = true ;

    tempLine = gLine ;
    tempColumn = gColumn ;

    tempString.push_back( nextChar ) ;

    peekChar = PeekNextChar() ;
    while ( IsWhiteSpace( peekChar ) == false && peekChar != '(' && peekChar != ')'
            && peekChar != ';' && peekChar != '\"' && peekChar != '\'' && peekChar != EOF ) {
      aloneDot = false ; // come in the while mean not alone DOT
      nextChar = GetNextChar() ;
      tempString.push_back( nextChar ) ;

      if ( nextChar == '.' )
        dotNum = dotNum + 1 ;
      else if ( nextChar == '+' || nextChar == '-' )
        posOrNegNum = posOrNegNum + 1 ;
      else if ( IsLetter( nextChar ) )
        hasLetter = true ;

      peekChar = PeekNextChar() ;
    } // while()
    
    if ( aloneDot == true ) {
      nextToken.content = tempString ;
      nextToken.type = DOT ;
      nextToken.mLine = tempLine ;
      nextToken.mColumn = tempColumn ;
    } // if()
    else if ( dotNum == 1 && posOrNegNum <= 1 && hasLetter == false ) {
      nextToken.content = tempString ;
      nextToken.type = FLOAT ;
      nextToken.mLine = tempLine ;
      nextToken.mColumn = tempColumn ;
    } // if()
    else {
      nextToken.content = tempString ;
      nextToken.type = SYMBOL ;
      nextToken.mLine = tempLine ;
      nextToken.mColumn = tempColumn ;
    } // else

  } // else if()
  // NIL or SYMBOL
  else if ( nextChar == 'n' ) {
    tempLine = gLine ;
    tempColumn = gColumn ;

    tempString.push_back( nextChar ) ;
    peekChar = PeekNextChar() ;

    while ( IsWhiteSpace( peekChar ) == false && peekChar != '(' && peekChar != ')'
            && peekChar != ';' && peekChar != '\"' && peekChar != '\'' && peekChar != EOF ) {
      nextChar = GetNextChar() ;
      tempString.push_back( nextChar ) ;
      
      peekChar = PeekNextChar() ;
    } // while()

    if ( tempString.compare( "nil" ) == 0 ) {
      nextToken.content = tempString ;
      nextToken.type = NIL ;
      nextToken.mLine = tempLine ;
      nextToken.mColumn = tempColumn ;
    } // if()
    else {
      nextToken.content = tempString ;
      nextToken.type = SYMBOL ;
      nextToken.mLine = tempLine ;
      nextToken.mColumn = tempColumn ;
    } // else

  } // else if()
  // T or SYMBOL
  else if ( nextChar == 't' ) {
    tempLine = gLine ;
    tempColumn = gColumn ;

    tempString.push_back( nextChar ) ;
    peekChar = PeekNextChar() ;

    while ( IsWhiteSpace( peekChar ) == false && peekChar != '(' && peekChar != ')'
            && peekChar != ';' && peekChar != '\"' && peekChar != '\'' && peekChar != EOF ) {
      nextChar = GetNextChar() ;
      tempString.push_back( nextChar ) ;
      
      peekChar = PeekNextChar() ;
    } // while()

    if ( tempString.compare( "t" ) == 0 ) {
      nextToken.content = "#t" ;
      nextToken.type = T ;
      nextToken.mLine = tempLine ;
      nextToken.mColumn = tempColumn ;
    } // if()
    else {
      nextToken.content = tempString ;
      nextToken.type = SYMBOL ;
      nextToken.mLine = tempLine ;
      nextToken.mColumn = tempColumn ;
    } // else

  } // else if()
  // QUOTE
  else if ( nextChar == '\'' ) {
    tempString.push_back( nextChar ) ;

    nextToken.content = "quote" ;
    nextToken.type = QUOTE ;
    nextToken.mLine = gLine ;
    nextToken.mColumn = gColumn ;
  } // else if()
  // comment
  else if ( nextChar == ';' ) {
    SkipComment() ;

    peekChar = PeekNextChar() ;
    GetToken( nextToken ) ;
  } // else if()
  // INT or FLOAT or NIL or T or SYMBOL
  else {
    tempLine = gLine ;
    tempColumn = gColumn ;

    int posOrNegNum = 0 ;
    int dotNum = 0 ;
    int digitNum = 0 ;
    bool isAllDigit = true ;

    tempString.push_back( nextChar ) ;

    if ( nextChar == '+' || nextChar == '-' ) 
      posOrNegNum = posOrNegNum + 1 ;
    else if ( nextChar == '.' )
      dotNum = dotNum + 1 ;
    else if ( IsDigit( nextChar ) == false )
      isAllDigit = false ;
    else if ( IsDigit( nextChar ) == true )
      digitNum = digitNum + 1 ;
    

    peekChar = PeekNextChar() ;
    while ( IsWhiteSpace( peekChar ) == false && peekChar != '(' && peekChar != ')'
            && peekChar != ';' && peekChar != '\"' && peekChar != '\'' && peekChar != EOF ) {
      nextChar = GetNextChar() ;
      tempString.push_back( nextChar ) ;

      if ( nextChar == '+' || nextChar == '-' ) 
        posOrNegNum = posOrNegNum + 1 ;
      else if ( nextChar == '.' )
        dotNum = dotNum + 1 ;
      else if ( IsDigit( nextChar ) == false )
        isAllDigit = false ;
      else if ( IsDigit( nextChar ) == true )
        digitNum = digitNum + 1 ;

      peekChar = PeekNextChar() ;
    } // while()

    if ( tempString.compare( "#f" ) == 0 ) {
      nextToken.content = "nil" ;
      nextToken.type = NIL ;
      nextToken.mLine = tempLine ;
      nextToken.mColumn = tempColumn ;
    } // if()
    else if ( tempString.compare( "#t" ) == 0  ) {
      nextToken.content = tempString ;
      nextToken.type = T ;
      nextToken.mLine = tempLine ;
      nextToken.mColumn = tempColumn ;
    } // else if()
    else if ( posOrNegNum <= 1 && dotNum == 0 && digitNum > 0 && isAllDigit == true ) {
      if ( tempString[0] == '+' ) 
        tempString.erase( 0, 1 ) ;
      
      nextToken.content = tempString ;
      nextToken.type = INT ;
      nextToken.mLine = tempLine ;
      nextToken.mColumn = tempColumn ;
    } // else if()
    else if ( posOrNegNum <= 1 && dotNum == 1 && digitNum > 0 && isAllDigit == true ) {
      if ( tempString[0] == '+' ) 
        tempString.erase( 0, 1 ) ;
      
      nextToken.content = tempString ;
      nextToken.type = FLOAT ;
      nextToken.mLine = tempLine ;
      nextToken.mColumn = tempColumn ;
    } // else if()
    else {
      nextToken.content = tempString ;
      nextToken.type = SYMBOL ;
      nextToken.mLine = tempLine ;
      nextToken.mColumn = tempColumn ;
    } // else

  } // else
} // GetToken()

bool IsAtom( Token aToken ) {
  if ( aToken.type ==  SYMBOL || aToken.type == INT || aToken.type == FLOAT 
       || aToken.type == STRING || aToken.type == NIL || aToken.type == T )
    return true ;
  else
    return false ;
} // IsAtom()

void ReadSExp( Token aToken, vector<Token> &sExpList ) {
  // if no aToken, get a new token
  if ( sExpList.size() == 0 )
    GetToken( aToken ) ;

  if ( aToken.type == LEFTPAREN ) {
    sExpList.push_back( aToken ) ;

    GetToken( aToken ) ;
    ReadSExp( aToken, sExpList ) ;

    GetToken( aToken ) ;
    while ( aToken.type != DOT && aToken.type != RIGHTPAREN ) {
      ReadSExp( aToken, sExpList ) ;

      GetToken( aToken ) ;
    } // while()

    if ( aToken.type == DOT ) {
      sExpList.push_back( aToken ) ;

      GetToken( aToken ) ;
      ReadSExp( aToken, sExpList ) ;

      GetToken( aToken ) ;

      if ( aToken.type == RIGHTPAREN ) {
        sExpList.push_back( aToken ) ;
      } // if()
      else {
        gerrorMsg = 2 ;
        throw ( aToken ) ;
      } // else
    } // if()
    else if ( aToken.type == RIGHTPAREN ) {
      Token tempToken ;
      tempToken.content = "." ;
      tempToken.type = DOT ;
      tempToken.mLine = -1 ;
      tempToken.mColumn = -1 ;
      sExpList.push_back( tempToken ) ;

      tempToken.content = "nil" ;
      tempToken.type = NIL ;
      tempToken.mLine = -1 ;
      tempToken.mColumn = -1 ;
      sExpList.push_back( tempToken ) ;

      sExpList.push_back( aToken ) ;
    } // else if()
    else {
      gerrorMsg = 2 ;
      throw ( aToken ) ;
    } // else
  } // if()
  else if ( aToken.type == QUOTE ) {
    
    sExpList.push_back( aToken ) ;

    GetToken( aToken ) ;
    ReadSExp( aToken, sExpList ) ;

  } // else if()
  else if ( IsAtom( aToken ) == true ) {
    sExpList.push_back( aToken ) ;
  } // else if()
  else {
    gerrorMsg = 1 ;
    throw ( aToken ) ;
  } // else
} // ReadSExp()

void GetSExpForTree( vector<Token> sExpList, int &i, vector<Token> &sExpForTeee ) {
  Token tempToken ;

  if ( IsAtom( sExpList[i] ) == true ) {
    sExpForTeee.push_back( sExpList[i] ) ;
    i = i + 1 ;
  } // if()
  else if ( sExpList[i].type == QUOTE ) {
    // QUOTE
    sExpForTeee.push_back( sExpList[i] ) ;
    i = i + 1 ;

    GetSExpForTree( sExpList, i, sExpForTeee ) ;

  } // else if()
  else {
    int addLP = 0 ;
    int addRP = 0 ;

    // LEFTPAREN
    sExpForTeee.push_back( sExpList[i] ) ;
    i = i + 1 ;

    while ( sExpList[i].type != DOT ) {
      
      GetSExpForTree( sExpList, i, sExpForTeee ) ;

      if ( sExpList[i].type != DOT ) {
        tempToken.content = "." ;
        tempToken.type = DOT ;
        tempToken.mLine = -1 ;
        tempToken.mColumn = -1 ;
        sExpForTeee.push_back( tempToken ) ;

        tempToken.content = "(" ;
        tempToken.type = LEFTPAREN ;
        tempToken.mLine = -1 ;
        tempToken.mColumn = -1 ;
        sExpForTeee.push_back( tempToken ) ;

        addLP = addLP + 1 ;
      } // if()

    } // while()
    
    // DOT
    sExpForTeee.push_back( sExpList[i] ) ;
    i = i + 1 ;

    GetSExpForTree( sExpList, i, sExpForTeee ) ;

    while ( addRP != addLP ) {
      tempToken.content = ")" ;
      tempToken.type = RIGHTPAREN ;
      tempToken.mLine = -1 ;
      tempToken.mColumn = -1 ;
      sExpForTeee.push_back( tempToken ) ;

      addRP = addRP + 1 ;
    } // while()
    // RIGHTTPAREN
    sExpForTeee.push_back( sExpList[i] ) ;
    i = i + 1 ;
  } // else

} // GetSExpForTree()

TreeNodePtr BuildTree( vector<Token> sExpForTree, int &i ) {

  if ( IsAtom( sExpForTree[i] ) == true ) {
    TreeNodePtr sExpTree = new TreeNode ;

    sExpTree->aToken = sExpForTree[i] ;
    sExpTree->isEmpty = false ;
    sExpTree->left = NULL ;

    sExpTree->right = NULL ;
    i = i + 1 ;

    return sExpTree ;
  } // if()
  else if ( sExpForTree[i].type == QUOTE ) {
    TreeNodePtr sExpTree = new TreeNode ;
    Token tempToken ;

    sExpTree->isEmpty = true ;

    sExpTree->left = new TreeNode ;
    sExpTree->left->isEmpty = false ;
    sExpTree->left->aToken = sExpForTree[i] ;
    sExpTree->left->left = NULL ;
    sExpTree->left->right = NULL ;
    i = i + 1 ;

    sExpTree->right = new TreeNode ;
    sExpTree->right->isEmpty = true ;
    sExpTree->right->left = BuildTree( sExpForTree, i ) ;
    sExpTree->right->right = NULL ;
    
    tempToken.content = "nil" ;
    tempToken.type = NIL ;
    tempToken.mLine = -1 ;
    tempToken.mColumn = -1 ;

    sExpTree->right->right = new TreeNode ;
    sExpTree->right->right->aToken = tempToken ;
    sExpTree->right->right->isEmpty = true ;
    sExpTree->right->right->left = NULL ;
    sExpTree->right->right->right = NULL ;
    
    return sExpTree ;
  } // else if()
  else {
    TreeNodePtr sExpTree = new TreeNode ;

    sExpTree->isEmpty = true ;
    i = i + 1 ; // read '('

    sExpTree->left = BuildTree( sExpForTree, i ) ;

    i = i + 1 ; // read '.'

    sExpTree->right = new TreeNode ;
    sExpTree->right->isEmpty = true ;
    sExpTree->right = BuildTree( sExpForTree, i ) ;

    i = i + 1 ; // read ')'

    return sExpTree ;
  } // else
} // BuildTree()


// ---------------------------- Eval's Function's Body  ----------------------------

void PrintTree( TreeNodePtr sExpTree, TreeNodePtr parentNode, vector<Token> &printList ) {
  if ( sExpTree != NULL ) {
    Token tempToken ;
    
    if ( sExpTree->isEmpty == true || sExpTree->aToken.content == "" ) {  // !!!!!!!!!!!!!! maybe error
      tempToken.content = "(" ;
      tempToken.type = LEFTPAREN ;
      tempToken.mLine = -1 ;
      tempToken.mColumn = -1 ;
      printList.push_back( tempToken ) ;
    } // if()
    else {
      printList.push_back( sExpTree->aToken ) ;
    } // else

    PrintTree( sExpTree->left, sExpTree, printList ) ;
    PrintRightTree( sExpTree->right, printList ) ;

    if ( parentNode != NULL && parentNode->right != NULL ) {
      if ( parentNode->right->isEmpty == false && parentNode->right->aToken.content != "" 
           && parentNode->right->aToken.type != NIL ) {
        tempToken.content = "." ;
        tempToken.type = DOT ;
        tempToken.mLine = -1 ;
        tempToken.mColumn = -1 ;
        printList.push_back( tempToken ) ;
      } // if()
    } // if()
  
  } // if()

} // PrintTree()

void PrintRightTree( TreeNodePtr sExpTree, vector<Token> &printList ) {  
  if ( sExpTree != NULL ) {
    Token tempToken ;
    
    if ( sExpTree->aToken.type != NIL && sExpTree->isEmpty == false )
      printList.push_back( sExpTree->aToken ) ;

    PrintTree( sExpTree->left, sExpTree, printList ) ;
    PrintRightTree( sExpTree->right, printList ) ;
    
    if ( sExpTree->left == NULL && sExpTree->right == NULL && sExpTree->aToken.type != RIGHTPAREN ) {
      tempToken.content = ")" ;
      tempToken.type = RIGHTPAREN ;
      tempToken.mLine = -1 ;
      tempToken.mColumn = -1 ;
      printList.push_back( tempToken ) ;
    } // if()
      
  } // if()

} // PrintRightTree()

void PrettyPrint( vector<Token> printList ) {
  int size = printList.size() ;
  int numWhite = 0 ;

  for ( int i = 0 ; i < size ; i ++ ) {
    if ( printList[i].content != "" ) {
      // print while

      if ( printList[i].type == LEFTPAREN ) {
        // print while
        for ( int j = 0 ; j < numWhite ; j ++ )
          cout << " " ;

        cout << printList[i].content << " " ;
        i = i + 1 ;

        numWhite = numWhite + 2 ;

        while ( printList[i].type == LEFTPAREN ) {
          cout << printList[i].content << " " ;
          i = i + 1 ;

          numWhite = numWhite + 2 ;
        } // while()

        if ( printList[i].type == FLOAT ) {
          float outputFloat = StringToFloat( printList[i].content ) ;
          printf( "%.3f\n", outputFloat ) ;
        } // if()
        else if ( ( printList[i].content == "define" || printList[i].content == "cons" 
                    || printList[i].content == "list" || printList[i].content == "quote" 
                    || printList[i].content == "car" || printList[i].content == "cdr" 
                    || printList[i].content == "atom?" || printList[i].content == "pair?" 
                    || printList[i].content == "list?" || printList[i].content == "null?" 
                    || printList[i].content == "integer?" || printList[i].content == "real?"
                    || printList[i].content == "number?" || printList[i].content == "string?" 
                    || printList[i].content == "boolean?" || printList[i].content == "symbol?" 
                    || printList[i].content == "+" || printList[i].content == "-" 
                    || printList[i].content == "*" || printList[i].content == "/" 
                    || printList[i].content == "not" || printList[i].content == "and" 
                    || printList[i].content == "or" || printList[i].content == ">" 
                    || printList[i].content == ">=" || printList[i].content == "<" 
                    || printList[i].content == "<=" || printList[i].content == "=" 
                    || printList[i].content == "string-append" || printList[i].content == "string>?" 
                    || printList[i].content == "string<?" || printList[i].content == "string=?" 
                    || printList[i].content == "eqv?" || printList[i].content == "equal?"
                    || printList[i].content == "begin" || printList[i].content == "if" 
                    || printList[i].content == "cond" || printList[i].content == "clean-environment" 
                    || printList[i].content == "exit" ) && gDontChange == false )
          cout << "#<procedure " << printList[i].content << ">" << endl ;
        else
          cout << printList[i].content << endl ;

      } // if()
      else if ( printList[i].type == RIGHTPAREN ) {
        numWhite = numWhite - 2 ;

        // print while
        for ( int j = 0 ; j < numWhite ; j ++ )
          cout << " " ;

        cout << printList[i].content << endl ;
      } // else if()
      else if ( ( printList[i].content == "define" || printList[i].content == "cons" 
                  || printList[i].content == "list" || printList[i].content == "quote" 
                  || printList[i].content == "car" || printList[i].content == "cdr" 
                  || printList[i].content == "atom?" || printList[i].content == "pair?" 
                  || printList[i].content == "list?" || printList[i].content == "null?" 
                  || printList[i].content == "integer?" || printList[i].content == "real?"
                  || printList[i].content == "number?" || printList[i].content == "string?" 
                  || printList[i].content == "boolean?" || printList[i].content == "symbol?" 
                  || printList[i].content == "+" || printList[i].content == "-" 
                  || printList[i].content == "*" || printList[i].content == "/" 
                  || printList[i].content == "not" || printList[i].content == "and" 
                  || printList[i].content == "or" || printList[i].content == ">" 
                  || printList[i].content == ">=" || printList[i].content == "<" 
                  || printList[i].content == "<=" || printList[i].content == "=" 
                  || printList[i].content == "string-append" || printList[i].content == "string>?" 
                  || printList[i].content == "string<?" || printList[i].content == "string=?" 
                  || printList[i].content == "eqv?" || printList[i].content == "equal?"
                  || printList[i].content == "begin" || printList[i].content == "if" 
                  || printList[i].content == "cond" || printList[i].content == "clean-environment" 
                  || printList[i].content == "exit" ) && gDontChange == false ) {
        
        for ( int j = 0 ; j < numWhite ; j ++ )
          cout << " " ;
        
        cout << "#<procedure " << printList[i].content << ">" << endl ;
      } // else if()
      else {
        // print while
        for ( int j = 0 ; j < numWhite ; j ++ )
          cout << " " ;

        if ( printList[i].type == FLOAT ) {
          float outputFloat = StringToFloat( printList[i].content ) ;
          printf( "%.3f\n", outputFloat ) ;
        } // if()
        else
          cout << printList[i].content << endl ;
      } // else
    } // if()
  } // for()

} // PrettyPrint()

void PrettyPrint2( vector<Token> printList ) {
  int size = printList.size() ;
  int numWhite = 0 ;

  for ( int i = 0 ; i < size ; i ++ ) {
    if ( printList[i].content != "" ) {
      // print while

      if ( printList[i].type == LEFTPAREN ) {
        // print while
        for ( int j = 0 ; j < numWhite ; j ++ )
          cout << " " ;

        cout << printList[i].content << " " ;
        i = i + 1 ;

        numWhite = numWhite + 2 ;

        while ( printList[i].type == LEFTPAREN ) {
          cout << printList[i].content << " " ;
          i = i + 1 ;

          numWhite = numWhite + 2 ;
        } // while()

        if ( printList[i].type == FLOAT ) {
          float outputFloat = StringToFloat( printList[i].content ) ;
          printf( "%.3f\n", outputFloat ) ;
        } // if()
        else
          cout << printList[i].content << endl ;

      } // if()
      else if ( printList[i].type == RIGHTPAREN ) {
        numWhite = numWhite - 2 ;

        // print while
        for ( int j = 0 ; j < numWhite ; j ++ )
          cout << " " ;

        cout << printList[i].content << endl ;
      } // else if()
      else {
        // print while
        for ( int j = 0 ; j < numWhite ; j ++ )
          cout << " " ;

        if ( printList[i].type == FLOAT ) {
          float outputFloat = StringToFloat( printList[i].content ) ;
          printf( "%.3f\n", outputFloat ) ;
        } // if()
        else
          cout << printList[i].content << endl ;
      } // else
    } // if()
  } // for()

} // PrettyPrint2()

string IntToString( int inputInt ) {
  stringstream ss ;
  string outputString ;

  ss << inputInt ;
  ss >> outputString ;

  return outputString ;
} // IntToString()

string FloatToString( float inputFloat ) {
  stringstream ss ;
  string outputString ;

  ss << fixed << setprecision( 3 ) << inputFloat ;
  ss >> outputString ;

  return outputString ;
} // FloatToString()

TreeNodePtr EvalSExp( TreeNodePtr inSExpTree ) {
  TreeNodePtr resultTree = new TreeNode ;
  vector<Token> printErrorList ;
  gLevel = gLevel + 1 ;

  if ( inSExpTree->left == NULL && inSExpTree->right == NULL 
       && IsAtom( inSExpTree->aToken ) == true && inSExpTree->aToken.type != SYMBOL ) {
    resultTree = inSExpTree ;
  } // if()
  else if ( inSExpTree->left == NULL && inSExpTree->right == NULL && inSExpTree->aToken.type == SYMBOL ) {
    bool hasBound = false ;
    int index = -1 ;
    string checkOriginal = inSExpTree->aToken.content ;

    hasBound = CheckBounded( inSExpTree->aToken.content, index ) ;

    if ( hasBound == true ) {
      resultTree = gVariable[index]->left ;
    } // if()
    else if ( checkOriginal == "define" || checkOriginal == "cons" || checkOriginal == "list"
              || checkOriginal == "quote" || checkOriginal == "car" || checkOriginal == "cdr" 
              || checkOriginal == "atom?" || checkOriginal == "pair?" || checkOriginal == "list?"
              || checkOriginal == "null?" || checkOriginal == "integer?" || checkOriginal == "real?"
              || checkOriginal == "number?" || checkOriginal == "string?" 
              || checkOriginal == "boolean?" || checkOriginal == "symbol?" || checkOriginal == "+"
              || checkOriginal == "-" || checkOriginal == "*" || checkOriginal == "/" 
              || checkOriginal == "not" || checkOriginal == "and" || checkOriginal == "or" 
              || checkOriginal == ">" || checkOriginal == ">=" || checkOriginal == "<" 
              || checkOriginal == "<=" || checkOriginal == "=" || checkOriginal == "string-append" 
              || checkOriginal == "string>?" || checkOriginal == "string<?" 
              || checkOriginal == "string=?" || checkOriginal == "eqv?" || checkOriginal == "equal?"
              || checkOriginal == "begin" || checkOriginal == "if" || checkOriginal == "cond" 
              || checkOriginal == "clean-environment" || checkOriginal == "exit" ) {
      resultTree = inSExpTree ;
    } // else if()
    else {
      resultTree = NULL ;
      gerrorEvalMsg = 4 ;
      PrintTree( inSExpTree, NULL, printErrorList ) ;
      throw ( printErrorList ) ;
    } // else 
  } // else if()
  else { // have ()
    string evalType = "" ;

    if ( inSExpTree->left->left != NULL )
      inSExpTree->left = EvalSExp( inSExpTree->left ) ;
    
    evalType = inSExpTree->left->aToken.content ;

    if ( inSExpTree->left->aToken.content == "" ) {
      resultTree = NULL ;
      gerrorEvalMsg = 2 ;
      PrintTree( inSExpTree->left, NULL, printErrorList ) ;
      throw ( printErrorList ) ;
    } // else if ()

    // use for check whether sExp is a list or not
    TreeNodePtr chcekList = new TreeNode ;
    chcekList = inSExpTree ;
    while ( chcekList->left != NULL && chcekList->right != NULL )
      chcekList = chcekList->right ;

    if ( chcekList->aToken.type != NIL ) {
      resultTree = NULL ;
      gerrorEvalMsg = 1 ;
      PrintTree( inSExpTree, NULL, printErrorList ) ;
      throw ( printErrorList ) ;
    } // if()
    else if ( IsAtom( inSExpTree->left->aToken ) == true && inSExpTree->left->aToken.type != SYMBOL ) {
      resultTree = NULL ;
      gerrorEvalMsg = 2 ;
      PrintTree( inSExpTree->left, NULL, printErrorList ) ;
      throw ( printErrorList ) ;
    } // else if()
    else if ( evalType == "define" ) {
      if ( gLevel != 1 ) {
        cout << "ERROR (level of DEFINE)" << endl ;
        
        resultTree = NULL ;
        gerrorEvalMsg = 10 ;
        PrintTree( inSExpTree, NULL, printErrorList ) ;
        throw ( printErrorList ) ;
      } // if()
      else {
        EvalDefine( inSExpTree ) ;
      } // else
      
      resultTree = NULL ;
    } // else if()
    else if ( evalType == "cons" ) {
      resultTree = EvalCons( inSExpTree ) ;
    } // else if()
    else if ( evalType == "list" ) {
      resultTree = EvalList( inSExpTree ) ;
    } // else if()
    else if ( evalType == "quote" ) { // "quote" and "'" have both become "quote"
      resultTree = EvalQuote( inSExpTree ) ;
      gDontChange = true ;
    } // else if()
    else if ( evalType == "car" ) {
      resultTree = EvalCar( inSExpTree ) ;
    } // else if()
    else if ( evalType == "cdr" ) {
      resultTree = EvalCdr( inSExpTree ) ;
    } // else if()
    else if ( evalType == "atom?" || evalType == "pair?" || evalType == "list?" 
              || evalType == "null?" || evalType == "integer?" || evalType == "real?" 
              || evalType == "number?" || evalType == "string?" || evalType == "boolean?" 
              || evalType == "symbol?" ) {
      resultTree = EvalPredicates( inSExpTree, evalType ) ;
    } // else if()
    else if ( evalType == "+" || evalType == "-" || evalType == "*" || evalType == "/" ) {
      resultTree = EvalArithmetic( inSExpTree, evalType ) ;
    } // else if()
    else if ( evalType == "not" ) {
      resultTree = EvalNot( inSExpTree ) ;
    } // else if()
    else if ( evalType == "and" ) {
      resultTree = EvalAnd( inSExpTree ) ;
    } // else if()
    else if ( evalType == "or" ) {
      resultTree = EvalOr( inSExpTree ) ;
    } // else if()
    else if ( evalType == ">" || evalType == ">=" || evalType == "<"
              || evalType == "<=" || evalType == "=" ) {
      resultTree = EvalBiggerSmaller( inSExpTree, evalType ) ;
    } // else if()
    else if ( evalType == "string-append" || evalType == "string>?" 
              || evalType == "string<?" || evalType == "string=?" ) {
      resultTree = EvalStringOpr( inSExpTree, evalType ) ;
    } // else if()
    else if ( evalType == "eqv?" || evalType == "equal?" ) {
      resultTree = EvalEq( inSExpTree, evalType ) ;
    } // else if()
    else if ( evalType == "begin" ) {
      resultTree = EvalBegin( inSExpTree ) ;
    } // else if()
    else if ( evalType == "if" ) {
      resultTree = EvalIf( inSExpTree ) ;
    } // else if()
    else if ( evalType == "cond" ) {
      resultTree = EvalCond( inSExpTree ) ;
    } // else if()
    else if ( evalType == "clean-environment" ) {
      if ( gLevel != 1 ) {
        cout << "ERROR (level of CLEAN-ENVIRONMENT)" << endl ;
        
        resultTree = NULL ;
        gerrorEvalMsg = 11 ;
        PrintTree( inSExpTree, NULL, printErrorList ) ;
        throw ( printErrorList ) ;
      } // if()
      else
        resultTree = EvalClean( inSExpTree ) ;

      resultTree = NULL ;
    } // else if()
    else if ( evalType == "exit" ) {
      if ( gLevel != 1 ) {
        cout << "ERROR (level of EXIT)" << endl ;
        
        resultTree = NULL ;
        gerrorEvalMsg = 12 ;
        PrintTree( inSExpTree, NULL, printErrorList ) ;
        throw ( printErrorList ) ;
      } // if()
      else
        resultTree = EvalExit( inSExpTree ) ;
    } // else if()
    else {
      bool hasBound = false ;
      int index = -1 ;

      hasBound = CheckBounded( inSExpTree->left->aToken.content, index ) ;

      if ( hasBound == true ) {
        string checkOriginal = gVariable[index]->left->aToken.content ;
        if ( checkOriginal == "define" || checkOriginal == "cons" || checkOriginal == "list"
             || checkOriginal == "quote" || checkOriginal == "car" || checkOriginal == "cdr" 
             || checkOriginal == "atom?" || checkOriginal == "pair?" || checkOriginal == "list?"
             || checkOriginal == "null?" || checkOriginal == "integer?" || checkOriginal == "real?"
             || checkOriginal == "number?" || checkOriginal == "string?" 
             || checkOriginal == "boolean?" || checkOriginal == "symbol?" || checkOriginal == "+"
             || checkOriginal == "-" || checkOriginal == "*" || checkOriginal == "/" 
             || checkOriginal == "not" || checkOriginal == "and" || checkOriginal == "or" 
             || checkOriginal == ">" || checkOriginal == ">=" || checkOriginal == "<" 
             || checkOriginal == "<=" || checkOriginal == "=" || checkOriginal == "string-append" 
             || checkOriginal == "string>?" || checkOriginal == "string<?" 
             || checkOriginal == "string=?" || checkOriginal == "eqv?" || checkOriginal == "equal?"
             || checkOriginal == "begin" || checkOriginal == "if" || checkOriginal == "cond" 
             || checkOriginal == "clean-environment" || checkOriginal == "exit" ) {
          
          inSExpTree->left = gVariable[index]->left ;
          resultTree = EvalSExp( inSExpTree ) ;
        } // if()
        else {
          gerrorEvalMsg = 2 ;
          PrintTree( gVariable[index]->left, NULL, printErrorList ) ;
          throw ( printErrorList ) ;
          resultTree = NULL ;
        } // else
      } // if()
      else {
        gerrorEvalMsg = 4 ;
        PrintTree( inSExpTree->left, NULL, printErrorList ) ;
        throw ( printErrorList ) ;
        resultTree = NULL ;
      } // else
    } // else
  } // else

  return resultTree ;
} // EvalSExp()

void EvalDefine( TreeNodePtr inSExpTree ) {
  vector<Token> printErrorList ;
  
  if ( inSExpTree->right->left == NULL ) { // no variable
    gerrorEvalMsg = 3 ;
    PrintTree( inSExpTree, NULL, printErrorList ) ;
    throw ( printErrorList ) ;
  } // if()
  else {
    Token variable ;
    variable = inSExpTree->right->left->aToken ;

    // check format
    // (define 5 x)
    if ( variable.type != SYMBOL ) {
      gerrorEvalMsg = 3 ;
      PrintTree( inSExpTree, NULL, printErrorList ) ;
      throw ( printErrorList ) ;
    } // if()
    // (define cons 5) ; attempt to redefine a system primitive
    else if ( variable.content == "define" || variable.content == "cons" || variable.content == "list"
              || variable.content == "quote" || variable.content == "car" || variable.content == "cdr" 
              || variable.content == "atom?" || variable.content == "pair?" || variable.content == "list?"
              || variable.content == "null?" || variable.content == "integer?" || variable.content == "real?"
              || variable.content == "number?" || variable.content == "string?" 
              || variable.content == "boolean?" || variable.content == "symbol?" || variable.content == "+"
              || variable.content == "-" || variable.content == "*" || variable.content == "/" 
              || variable.content == "not" || variable.content == "and" || variable.content == "or" 
              || variable.content == ">" || variable.content == ">=" || variable.content == "<" 
              || variable.content == "<=" || variable.content == "=" || variable.content == "string-append" 
              || variable.content == "string>?" || variable.content == "string<?" 
              || variable.content == "string=?" || variable.content == "eqv?" || variable.content == "equal?"
              || variable.content == "begin" || variable.content == "if" || variable.content == "cond" 
              || variable.content == "clean-environment" || variable.content == "exit" ) {
      gerrorEvalMsg = 3 ;
      PrintTree( inSExpTree, NULL, printErrorList ) ;
      throw ( printErrorList ) ;
    } // if() 
    // (define a)
    else if ( inSExpTree->right->right->aToken.type == NIL ) {
      gerrorEvalMsg = 3 ;
      PrintTree( inSExpTree, NULL, printErrorList ) ;
      throw ( printErrorList ) ;
    } // else if()
    // (define a 10 20)
    else if ( inSExpTree->right->right->right->aToken.type != NIL ) {
      gerrorEvalMsg = 3 ;
      PrintTree( inSExpTree, NULL, printErrorList ) ;
      throw ( printErrorList ) ;
    } // else if()
    else {
      TreeNodePtr tempTree = new TreeNode ; // to eval S-Exp
      int size = gVariable.size() ;
      bool boundBefore = false ;
      int boundPos = -1 ;

      tempTree = EvalSExp( inSExpTree->right->right->left ) ;

      for ( int i = 0 ; i < size ; i ++ ) {
        if ( inSExpTree->right->left->aToken.content == gVariable[i]->aToken.content ) {
          boundBefore = true ; // has bounded before
          boundPos = i ;
        } // if()
      } // for()

      if ( boundBefore == true ) {
        gVariable[boundPos]->left = tempTree ;
        gVariable[boundPos]->right = NULL ;

        cout << gVariable[boundPos]->aToken.content << " defined" << endl ;
      } // if()
      else {
        gVariable.push_back( inSExpTree->right->left ) ;

        gVariable.back()->left = tempTree ;
        gVariable.back()->right = NULL ;

        cout << gVariable.back()->aToken.content << " defined" << endl ;
      } // else
    } // else
  } // else

} // EvalDefine()

bool CheckBounded( string variable, int &index ) {
  int size = gVariable.size() ;

  for ( int i = 0 ; i < size ; i ++ ) {
    if ( gVariable[i]->aToken.content == variable ) {
      index = i ;
      return true ;
    } // if()
  } // for()

  return false ;
} // CheckBounded()

TreeNodePtr EvalQuote( TreeNodePtr inSExpTree ) {
  TreeNodePtr resultTree = new TreeNode ;
  vector<Token> printErrorList ;
  
  // check number of arguments
  if ( inSExpTree->right->left == NULL ) {
    resultTree = NULL ;
    
    gerrorEvalMsg = 5 ;
    cout << "ERROR (incorrect number of arguments) : " << inSExpTree->left->aToken.content << endl ;
    PrintTree( inSExpTree->left, NULL, printErrorList ) ;
    throw ( printErrorList ) ;
  } // if()
  else if ( inSExpTree->right->right->left != NULL ) {
    resultTree = NULL ;
    
    gerrorEvalMsg = 5 ;
    cout << "ERROR (incorrect number of arguments) : " << inSExpTree->left->aToken.content << endl ;
    PrintTree( inSExpTree->left, NULL, printErrorList ) ;
    throw ( printErrorList ) ;
  } // else if()
  else {
    resultTree = inSExpTree->right->left ;
  } // else

  return resultTree ;
} // EvalQuote()

TreeNodePtr EvalCons( TreeNodePtr inSExpTree ) {
  TreeNodePtr resultTree = new TreeNode ;
  vector<Token> printErrorList ;

  // check number of arguments
  if ( inSExpTree->right->left == NULL ) {
    resultTree = NULL ;
    
    gerrorEvalMsg = 5 ;
    cout << "ERROR (incorrect number of arguments) : " << inSExpTree->left->aToken.content << endl ;
    PrintTree( inSExpTree->left, NULL, printErrorList ) ;
    throw ( printErrorList ) ;
  } // if()
  else if ( inSExpTree->right->right->left == NULL ) {
    resultTree = NULL ;
    
    gerrorEvalMsg = 5 ;
    cout << "ERROR (incorrect number of arguments) : " << inSExpTree->left->aToken.content << endl ;
    PrintTree( inSExpTree->left, NULL, printErrorList ) ;
    throw ( printErrorList ) ;
  } // if
  else if ( inSExpTree->right->right->right->left != NULL ) {
    resultTree = NULL ;
    
    gerrorEvalMsg = 5 ;
    cout << "ERROR (incorrect number of arguments) : " << inSExpTree->left->aToken.content << endl ;
    PrintTree( inSExpTree->left, NULL, printErrorList ) ;
    throw ( printErrorList ) ;
  } // else if()
  else {
    resultTree->left = EvalSExp( inSExpTree->right->left ) ;
    resultTree->right = EvalSExp( inSExpTree->right->right->left ) ;
  } // else

  return resultTree ;
} // EvalCons()

TreeNodePtr EvalList( TreeNodePtr inSExpTree ) {
  TreeNodePtr resultTree = new TreeNode ;
  TreeNodePtr walk = new TreeNode ;

  if ( inSExpTree->right == NULL ) // (list)
    resultTree = NULL ;
  else {
    resultTree = inSExpTree->right ;
    walk = resultTree ;

    while ( walk->left != NULL ) {
      walk->left = EvalSExp( walk->left ) ;
      walk = walk->right ;
    } // while()
  } // else
  
  return resultTree ;
} // EvalList()

TreeNodePtr EvalCar( TreeNodePtr inSExpTree ) {
  TreeNodePtr resultTree = new TreeNode ;
  TreeNodePtr tempTree = new TreeNode ;
  vector<Token> printErrorList ;

  // check number of arguments
  if ( inSExpTree->right->left == NULL ) { // no arguments
    resultTree = NULL ;
    
    gerrorEvalMsg = 5 ;
    cout << "ERROR (incorrect number of arguments) : " << inSExpTree->left->aToken.content << endl ;
    PrintTree( inSExpTree->left, NULL, printErrorList ) ;
    throw ( printErrorList ) ;
  } // if()
  else if ( inSExpTree->right->right->aToken.type != NIL ) {
    resultTree = NULL ;
    
    gerrorEvalMsg = 5 ;
    cout << "ERROR (incorrect number of arguments) : " << inSExpTree->left->aToken.content << endl ;
    PrintTree( inSExpTree->left, NULL, printErrorList ) ;
    throw ( printErrorList ) ;
  } // else if()
  else {
    tempTree = EvalSExp( inSExpTree->right->left ) ;

    if ( tempTree->left == NULL && tempTree->right == NULL ) {
      resultTree = NULL ;
    
      gerrorEvalMsg = 6 ;
      cout << "ERROR (" << inSExpTree->left->aToken.content ;
      PrintTree( tempTree, NULL, printErrorList ) ;
      throw ( printErrorList ) ;
    } // if()
    else {
      resultTree = tempTree->left ;
    } // else
  } // else
  
  return resultTree ;
} // EvalCar()

TreeNodePtr EvalCdr( TreeNodePtr inSExpTree ) {
  TreeNodePtr resultTree = new TreeNode ;
  TreeNodePtr tempTree = new TreeNode ;
  vector<Token> printErrorList ;

  // check number of arguments
  if ( inSExpTree->right->left == NULL ) { // no arguments
    resultTree = NULL ;
    
    gerrorEvalMsg = 5 ;
    cout << "ERROR (incorrect number of arguments) : " << inSExpTree->left->aToken.content << endl ;
    PrintTree( inSExpTree->left, NULL, printErrorList ) ;
    throw ( printErrorList ) ;
  } // if()
  else if ( inSExpTree->right->right->aToken.type != NIL ) {
    resultTree = NULL ;
    
    gerrorEvalMsg = 5 ;
    cout << "ERROR (incorrect number of arguments) : " << inSExpTree->left->aToken.content << endl ;
    PrintTree( inSExpTree->left, NULL, printErrorList ) ;
    throw ( printErrorList ) ;
  } // else if()
  else {
    tempTree = EvalSExp( inSExpTree->right->left ) ;

    if ( tempTree->left == NULL && tempTree->right == NULL ) {
      resultTree = NULL ;
    
      gerrorEvalMsg = 6 ;
      cout << "ERROR (" << inSExpTree->left->aToken.content ;
      PrintTree( tempTree, NULL, printErrorList ) ;
      throw ( printErrorList ) ;
    } // if()
    else {
      resultTree = tempTree->right ;
    } // else
  } // else
  
  return resultTree ;
} // EvalCdr()

TreeNodePtr EvalPredicates( TreeNodePtr inSExpTree, string evalType ) {
  TreeNodePtr resultTree = new TreeNode ;
  vector<Token> printErrorList ;

  // check number of arguments
  if ( inSExpTree->right->left == NULL ) { // no arguments
    resultTree = NULL ;
    
    gerrorEvalMsg = 5 ;
    cout << "ERROR (incorrect number of arguments) : " << inSExpTree->left->aToken.content << endl ;
    PrintTree( inSExpTree->left, NULL, printErrorList ) ;
    throw ( printErrorList ) ;
  } // if()
  else if ( inSExpTree->right->right->left != NULL ) {
    resultTree = NULL ;
    
    gerrorEvalMsg = 5 ;
    cout << "ERROR (incorrect number of arguments) : " << inSExpTree->left->aToken.content << endl ;
    PrintTree( inSExpTree->left, NULL, printErrorList ) ;
    throw ( printErrorList ) ;
  } // else if()
  else {
    bool answer = false ;
    TreeNodePtr tempTree = new TreeNode ;
    tempTree = EvalSExp( inSExpTree->right->left ) ;

    if ( evalType == "atom?" ) {
      if ( tempTree->left == NULL && tempTree->right == NULL ) {
        if ( IsAtom( tempTree->aToken ) == true )
          answer = true ;
        else
          answer = false ;
      } // if()
      else
        answer = false ;
      
    } // if() 
    else if ( evalType == "pair?" ) {
      if ( tempTree->left == NULL && tempTree->right == NULL ) {
        if ( IsAtom( tempTree->aToken ) == true )
          answer = false ;
        else
          answer = true ;
      }  // if()
      else 
        answer = true ;
    } // else if()
    else if ( evalType == "list?" ) {
      TreeNodePtr walk = new TreeNode ;
      walk = tempTree ;

      if ( walk == NULL )
        answer = false ;
      else {
        while ( walk->left != NULL && walk->right != NULL )
          walk = walk->right ;

        if ( walk->aToken.type == NIL ) 
          answer = true ;
        else 
          answer = false ;
      } // else
    } // else if()
    else if ( evalType == "null?" ) {
      if ( tempTree->aToken.type == NIL && tempTree->left == NULL && tempTree->right == NULL )
        answer = true ;
      else
        answer = false ;
    } // else if() 
    else if ( evalType == "integer?" ) {
      if ( tempTree->left == NULL && tempTree->right == NULL &&  tempTree->aToken.type == INT )
        answer = true ;
      else
        answer = false ;
    } // else if() 
    else if ( evalType == "real?" || evalType == "number?" ) {
      if ( tempTree->left == NULL && tempTree->right == NULL 
           && ( tempTree->aToken.type == INT || tempTree->aToken.type == FLOAT ) )
        answer = true ;
      else
        answer = false ;
    } // else if()
    else if ( evalType == "string?" ) {
      if ( tempTree->left == NULL && tempTree->right == NULL && tempTree->aToken.type == STRING )
        answer = true ;
      else
        answer = false ;
    } // else if()
    else if ( evalType == "boolean?" ) {
      if ( tempTree->left == NULL && tempTree->right == NULL 
           && ( tempTree->aToken.type == T || tempTree->aToken.type == NIL ) )
        answer = true ;
      else
        answer = false ;
    } // else if()
    else if ( evalType == "symbol?" ) {
      if ( tempTree->left == NULL && tempTree->right == NULL && tempTree->aToken.type == SYMBOL )
        answer = true ;
      else
        answer = false ;
    } // else if()

    // make resultTree
    if ( answer == true ) {
      Token tempToken ;
      tempToken.content = "#t" ;
      tempToken.type = T ;
      tempToken.mLine = -1 ;
      tempToken.mColumn = -1 ;

      resultTree->aToken = tempToken ;
      resultTree->isEmpty = false ;
      resultTree->left = NULL ;
      resultTree->right = NULL ;
    } // if()
    else {
      Token tempToken ;
      tempToken.content = "nil" ;
      tempToken.type = NIL ;
      tempToken.mLine = -1 ;
      tempToken.mColumn = -1 ;

      resultTree->aToken = tempToken ;
      resultTree->isEmpty = false ;
      resultTree->left = NULL ;
      resultTree->right = NULL ;
    } // else
  } // else

  return resultTree ;
} // EvalPredicates()

TreeNodePtr EvalArithmetic( TreeNodePtr inSExpTree, string evalType ) {
  TreeNodePtr resultTree = new TreeNode ;
  vector<Token> printErrorList ;

  // check number of arguments
  if ( inSExpTree->right->left == NULL ) {
    resultTree = NULL ;
    
    gerrorEvalMsg = 5 ;
    cout << "ERROR (incorrect number of arguments) : " << inSExpTree->left->aToken.content << endl ;
    PrintTree( inSExpTree->left, NULL, printErrorList ) ;
    throw ( printErrorList ) ;
  } // if()
  else if ( inSExpTree->right->right->left == NULL ) {
    resultTree = NULL ;
    
    gerrorEvalMsg = 5 ;
    cout << "ERROR (incorrect number of arguments) : " << inSExpTree->left->aToken.content << endl ;
    PrintTree( inSExpTree->left, NULL, printErrorList ) ;
    throw ( printErrorList ) ;
  } // if
  else {
    vector<Token> operand ;
    TreeNodePtr walk = new TreeNode ;
    TreeNodePtr tempTree = new TreeNode ;
    bool isFloat = false ; // false is "all int", true is "has float"

    walk = inSExpTree->right ;

    while ( walk->left != NULL ) {
      tempTree = EvalSExp( walk->left ) ;

      if ( tempTree->aToken.type == INT || tempTree->aToken.type == FLOAT ) {
        if ( tempTree->aToken.type == FLOAT )
          isFloat = true ;
        
        operand.push_back( tempTree->aToken ) ;
      } // if()
      else { // error type
        gerrorEvalMsg = 6 ;
        cout << "ERROR (" << inSExpTree->left->aToken.content ;
        PrintTree( tempTree, NULL, printErrorList ) ;
        throw ( printErrorList ) ;
      } // else

      walk = walk->right ;
    } // while()

    int answerInt = 0 ;
    float answerFloat = 0.0 ;
    int size = operand.size() ;

    if ( isFloat == true ) {
      answerFloat = StringToFloat( operand[0].content ) ;

      if ( evalType == "+" ) {
        for ( int i = 1 ; i < size ; i ++ ) 
          answerFloat = answerFloat + StringToFloat( operand[i].content ) ;
      } // if()
      else if ( evalType == "-" ) {
        for ( int i = 1 ; i < size ; i ++ ) 
          answerFloat = answerFloat - StringToFloat( operand[i].content ) ;
      } // else if()
      else if ( evalType == "*" ) {
        for ( int i = 1 ; i < size ; i ++ ) 
          answerFloat = answerFloat * StringToFloat( operand[i].content ) ;
      } // else if()
      else {
        for ( int i = 1 ; i < size ; i ++ ) {
          if ( operand[i].content == "0" ) {
            gerrorEvalMsg = 7 ;
            PrintTree( inSExpTree->left, NULL, printErrorList ) ;
            throw ( printErrorList ) ;
          } // if()
          else
            answerFloat = answerFloat / StringToFloat( operand[i].content ) ;
        } // for()
      } // else if()
    } // if()
    else {
      answerInt = StringToInt( operand[0].content ) ;

      if ( evalType == "+" ) {
        for ( int i = 1 ; i < size ; i ++ ) 
          answerInt = answerInt + StringToInt( operand[i].content ) ;
      } // if()
      else if ( evalType == "-" ) {
        for ( int i = 1 ; i < size ; i ++ ) 
          answerInt = answerInt - StringToInt( operand[i].content ) ;
      } // else if()
      else if ( evalType == "*" ) {
        for ( int i = 1 ; i < size ; i ++ ) 
          answerInt = answerInt * StringToInt( operand[i].content ) ;
      } // else if()
      else {
        for ( int i = 1 ; i < size ; i ++ ) {
          if ( operand[i].content == "0" ) {
            gerrorEvalMsg = 7 ;
            PrintTree( inSExpTree->left, NULL, printErrorList ) ;
            throw ( printErrorList ) ;
          } // if()
          else
            answerInt = answerInt / StringToInt( operand[i].content ) ;
        } // for()
      } // else if()
    } // else

    Token tempToken ;
    if ( isFloat == true ) {
      tempToken.content = FloatToString( answerFloat ) ;
      tempToken.type = FLOAT ;
      tempToken.mLine = -1 ;
      tempToken.mColumn = -1 ;

      resultTree->aToken = tempToken ;
      resultTree->isEmpty = false ;
      resultTree->left = NULL ;
      resultTree->right = NULL ;
    } // if()
    else {
      tempToken.content = IntToString( answerInt ) ;
      tempToken.type = INT ;
      tempToken.mLine = -1 ;
      tempToken.mColumn = -1 ;

      resultTree->aToken = tempToken ;
      resultTree->isEmpty = false ;
      resultTree->left = NULL ;
      resultTree->right = NULL ;
    } // else
  } // else
  
  return resultTree ;
} // EvalArithmetic()

TreeNodePtr EvalNot( TreeNodePtr inSExpTree ) {
  TreeNodePtr resultTree = new TreeNode ;
  vector<Token> printErrorList ;

  // check number of arguments
  if ( inSExpTree->right->left == NULL ) { // no arguments
    resultTree = NULL ;
    
    gerrorEvalMsg = 5 ;
    cout << "ERROR (incorrect number of arguments) : " << inSExpTree->left->aToken.content << endl ;
    PrintTree( inSExpTree->left, NULL, printErrorList ) ;
    throw ( printErrorList ) ;
  } // if()
  else if ( inSExpTree->right->right->aToken.type != NIL ) {
    resultTree = NULL ;
    
    gerrorEvalMsg = 5 ;
    cout << "ERROR (incorrect number of arguments) : " << inSExpTree->left->aToken.content << endl ;
    PrintTree( inSExpTree->left, NULL, printErrorList ) ;
    throw ( printErrorList ) ;
  } // else if()
  else {
    Token tempToken ;
    TreeNodePtr tempTree = new TreeNode ;

    tempTree = EvalSExp( inSExpTree->right->left ) ;

    if ( tempTree->aToken.type == NIL ) {
      tempToken.content = "#t" ;
      tempToken.type = T ;
      tempToken.mLine = -1 ;
      tempToken.mColumn = -1 ;

      resultTree->aToken = tempToken ;
      resultTree->isEmpty = false ;
      resultTree->left = NULL ;
      resultTree->right = NULL ;
    } // if()
    else {
      tempToken.content = "nil" ;
      tempToken.type = NIL ;
      tempToken.mLine = -1 ;
      tempToken.mColumn = -1 ;

      resultTree->aToken = tempToken ;
      resultTree->isEmpty = false ;
      resultTree->left = NULL ;
      resultTree->right = NULL ;
    } // else
  } // else

  return resultTree ;
} // EvalNot()

TreeNodePtr EvalAnd( TreeNodePtr inSExpTree ) {
  TreeNodePtr resultTree = new TreeNode ;
  vector<Token> printErrorList ;

  // check number of arguments
  if ( inSExpTree->right->left == NULL ) {
    resultTree = NULL ;
    
    gerrorEvalMsg = 5 ;
    cout << "ERROR (incorrect number of arguments) : " << inSExpTree->left->aToken.content << endl ;
    PrintTree( inSExpTree->left, NULL, printErrorList ) ;
    throw ( printErrorList ) ;
  } // if()
  else if ( inSExpTree->right->right->left == NULL ) {
    resultTree = NULL ;
    
    gerrorEvalMsg = 5 ;
    cout << "ERROR (incorrect number of arguments) : " << inSExpTree->left->aToken.content << endl ;
    PrintTree( inSExpTree->left, NULL, printErrorList ) ;
    throw ( printErrorList ) ;
  } // if
  else {
    TreeNodePtr walk = new TreeNode ;
    TreeNodePtr tempTree = new TreeNode ;

    walk = inSExpTree->right ;

    while ( walk->left != NULL ) {
      tempTree = EvalSExp( walk->left ) ;

      if ( tempTree->aToken.type == NIL ) {
        resultTree = tempTree ;
        return resultTree ;
      } // if ()

      walk = walk->right ;
    } // while()

    resultTree = tempTree ;
  } // else()

  return resultTree ;
} // EvalAnd()

TreeNodePtr EvalOr( TreeNodePtr inSExpTree ) {
  TreeNodePtr resultTree = new TreeNode ;
  vector<Token> printErrorList ;

  // check number of arguments
  if ( inSExpTree->right->left == NULL ) {
    resultTree = NULL ;
    
    gerrorEvalMsg = 5 ;
    cout << "ERROR (incorrect number of arguments) : " << inSExpTree->left->aToken.content << endl ;
    PrintTree( inSExpTree->left, NULL, printErrorList ) ;
    throw ( printErrorList ) ;
  } // if()
  else if ( inSExpTree->right->right->left == NULL ) {
    resultTree = NULL ;
    
    gerrorEvalMsg = 5 ;
    cout << "ERROR (incorrect number of arguments) : " << inSExpTree->left->aToken.content << endl ;
    PrintTree( inSExpTree->left, NULL, printErrorList ) ;
    throw ( printErrorList ) ;
  } // if
  else {
    TreeNodePtr walk = new TreeNode ;
    TreeNodePtr tempTree = new TreeNode ;

    walk = inSExpTree->right ;

    while ( walk->left != NULL ) {
      tempTree = EvalSExp( walk->left ) ;

      if ( tempTree->aToken.type != NIL ) {
        resultTree = tempTree ;
        return resultTree ;
      } // if ()

      walk = walk->right ;
    } // while()

    resultTree = tempTree ;
  } // else()

  return resultTree ;
} // EvalOr()

TreeNodePtr EvalBiggerSmaller( TreeNodePtr inSExpTree, string evalType ) {
  TreeNodePtr resultTree = new TreeNode ;
  vector<Token> printErrorList ;

  // check number of arguments
  if ( inSExpTree->right->left == NULL ) {
    resultTree = NULL ;
    
    gerrorEvalMsg = 5 ;
    cout << "ERROR (incorrect number of arguments) : " << inSExpTree->left->aToken.content << endl ;
    PrintTree( inSExpTree->left, NULL, printErrorList ) ;
    throw ( printErrorList ) ;
  } // if()
  else if ( inSExpTree->right->right->left == NULL ) {
    resultTree = NULL ;
    
    gerrorEvalMsg = 5 ;
    cout << "ERROR (incorrect number of arguments) : " << inSExpTree->left->aToken.content << endl ;
    PrintTree( inSExpTree->left, NULL, printErrorList ) ;
    throw ( printErrorList ) ;
  } // if
  else {
    vector<Token> operand ;
    TreeNodePtr walk = new TreeNode ;
    TreeNodePtr tempTree = new TreeNode ;
    bool isFloat = false ;

    walk = inSExpTree->right ;

    while ( walk->left != NULL ) {
      tempTree = EvalSExp( walk->left ) ;

      if ( tempTree->aToken.type == INT || tempTree->aToken.type == FLOAT ) {
        operand.push_back( tempTree->aToken ) ;

        if ( tempTree->aToken.type == FLOAT )
          isFloat = true ;
      } // if()
      else { // error type
        gerrorEvalMsg = 6 ;
        cout << "ERROR (" << inSExpTree->left->aToken.content ;
        PrintTree( tempTree, NULL, printErrorList ) ;
        throw ( printErrorList ) ;
      } // else

      walk = walk->right ;
    } // while()

    int size = operand.size() ;
    int answer = 1 ;
    vector<int> intOperand ;
    vector<float> floatOperand ;

    if ( isFloat == false ) {
      for ( int i = 0 ; i < size ; i ++ ) 
        intOperand.push_back( StringToInt( operand[i].content ) ) ;
      

      if ( evalType == ">" ) {
        for ( int i = 1 ; i < size ; i ++ ) {
          if ( intOperand[i-1] > intOperand[i] )
            answer = answer * 1 ;
          else
            answer = answer * 0 ;
        } // for()
      } // if()
      else if ( evalType == ">=" ) {
        for ( int i = 1 ; i < size ; i ++ ) {
          if ( intOperand[i-1] >= intOperand[i] )
            answer = answer * 1 ;
          else
            answer = answer * 0 ;
        } // for()
      } // else if()
      else if ( evalType == "<" ) {
        for ( int i = 1 ; i < size ; i ++ ) {
          if ( intOperand[i-1] < intOperand[i] )
            answer = answer * 1 ;
          else
            answer = answer * 0 ;
        } // for()
      } // else if()
      else if ( evalType == "<=" ) {
        for ( int i = 1 ; i < size ; i ++ ) {
          if ( intOperand[i-1] <= intOperand[i] )
            answer = answer * 1 ;
          else
            answer = answer * 0 ;
        } // for()
      } // else if()
      else if ( evalType == "=" ) {
        for ( int i = 1 ; i < size ; i ++ ) {
          if ( intOperand[i-1] == intOperand[i] )
            answer = answer * 1 ;
          else
            answer = answer * 0 ;
        } // for()
      } // else if()
    } // if()
    else { // isFloat == true
      for ( int i = 0 ; i < size ; i ++ ) 
        floatOperand.push_back( StringToFloat( operand[i].content ) ) ;

      if ( evalType == ">" ) {
        for ( int i = 1 ; i < size ; i ++ ) {
          if ( floatOperand[i-1] > floatOperand[i] )
            answer = answer * 1 ;
          else
            answer = answer * 0 ;
        } // for()
      } // if()
      else if ( evalType == ">=" ) {
        for ( int i = 1 ; i < size ; i ++ ) {
          if ( floatOperand[i-1] >= floatOperand[i] )
            answer = answer * 1 ;
          else
            answer = answer * 0 ;
        } // for()
      } // else if()
      else if ( evalType == "<" ) {
        for ( int i = 1 ; i < size ; i ++ ) {
          if ( floatOperand[i-1] < floatOperand[i] )
            answer = answer * 1 ;
          else
            answer = answer * 0 ;
        } // for()
      } // else if()
      else if ( evalType == "<=" ) {
        for ( int i = 1 ; i < size ; i ++ ) {
          if ( floatOperand[i-1] <= floatOperand[i] )
            answer = answer * 1 ;
          else
            answer = answer * 0 ;
        } // for()
      } // else if()
      else if ( evalType == "=" ) {
        for ( int i = 1 ; i < size ; i ++ ) {
          if ( floatOperand[i-1] == floatOperand[i] )
            answer = answer * 1 ;
          else
            answer = answer * 0 ;
        } // for()
      } // else if()
    } // else

    // make resultTree
    if ( answer == 1 ) {
      Token tempToken ;
      tempToken.content = "#t" ;
      tempToken.type = T ;
      tempToken.mLine = -1 ;
      tempToken.mColumn = -1 ;

      resultTree->aToken = tempToken ;
      resultTree->isEmpty = false ;
      resultTree->left = NULL ;
      resultTree->right = NULL ;
    } // if()
    else {
      Token tempToken ;
      tempToken.content = "nil" ;
      tempToken.type = NIL ;
      tempToken.mLine = -1 ;
      tempToken.mColumn = -1 ;

      resultTree->aToken = tempToken ;
      resultTree->isEmpty = false ;
      resultTree->left = NULL ;
      resultTree->right = NULL ;
    } // else
  } // else()

  return resultTree ;
} // EvalBiggerSmaller()

TreeNodePtr EvalStringOpr( TreeNodePtr inSExpTree, string evalType ) {
  TreeNodePtr resultTree = new TreeNode ;
  vector<Token> printErrorList ;

  // check number of arguments
  if ( inSExpTree->right->left == NULL ) {
    resultTree = NULL ;
    
    gerrorEvalMsg = 5 ;
    cout << "ERROR (incorrect number of arguments) : " << inSExpTree->left->aToken.content << endl ;
    PrintTree( inSExpTree->left, NULL, printErrorList ) ;
    throw ( printErrorList ) ;
  } // if()
  else if ( inSExpTree->right->right->left == NULL ) {
    resultTree = NULL ;
    
    gerrorEvalMsg = 5 ;
    cout << "ERROR (incorrect number of arguments) : " << inSExpTree->left->aToken.content << endl ;
    PrintTree( inSExpTree->left, NULL, printErrorList ) ;
    throw ( printErrorList ) ;
  } // if
  else {
    vector<Token> operand ;
    TreeNodePtr walk = new TreeNode ;
    TreeNodePtr tempTree = new TreeNode ;

    walk = inSExpTree->right ;

    while ( walk->left != NULL ) {
      tempTree = EvalSExp( walk->left ) ;

      if ( tempTree->aToken.type == STRING )
        operand.push_back( tempTree->aToken ) ;
      else { // error type
        gerrorEvalMsg = 6 ;
        cout << "ERROR (" << inSExpTree->left->aToken.content ;
        PrintTree( tempTree, NULL, printErrorList ) ;
        throw ( printErrorList ) ;
      } // else

      walk = walk->right ;
    } // while()

    int size = operand.size() ;

    if ( evalType == "string-append" ) {
      Token tempToken ;
      string tempString = operand[0].content ;

      for ( int i = 1 ; i < size ; i ++ ) {
        int stringSize = tempString.size() ;
        tempString = tempString.erase(  stringSize - 1, 1 ) + operand[i].content.erase( 0, 1 ) ;
      } // for()

      tempToken.content = tempString ;
      tempToken.type = STRING ;
      tempToken.mLine = -1 ;
      tempToken.mColumn = -1 ;

      resultTree->aToken = tempToken ;
      resultTree->isEmpty = false ;
      resultTree->left = NULL ;
      resultTree->right = NULL ;
    } // if()
    else {
      int answer = 1 ;
      
      if ( evalType == "string>?" ) {
        for ( int i = 1 ; i < size ; i ++ ) {
          if ( operand[i-1].content > operand[i].content )
            answer = answer * 1 ;
          else
            answer = answer * 0 ;
        } // for()
      } // if()
      else if ( evalType == "string<?" ) {
        for ( int i = 1 ; i < size ; i ++ ) {
          if ( operand[i-1].content < operand[i].content )
            answer = answer * 1 ;
          else
            answer = answer * 0 ;
        } // for()
      } // else if()
      else if ( evalType == "string=?" ) {
        for ( int i = 1 ; i < size ; i ++ ) {
          if ( operand[i-1].content == operand[i].content )
            answer = answer * 1 ;
          else
            answer = answer * 0 ;
        } // for()
      } // else if()
      
      // make resultTree
      if ( answer == 1 ) {
        Token tempToken ;
        tempToken.content = "#t" ;
        tempToken.type = T ;
        tempToken.mLine = -1 ;
        tempToken.mColumn = -1 ;

        resultTree->aToken = tempToken ;
        resultTree->isEmpty = false ;
        resultTree->left = NULL ;
        resultTree->right = NULL ;
      } // if()
      else {
        Token tempToken ;
        tempToken.content = "nil" ;
        tempToken.type = NIL ;
        tempToken.mLine = -1 ;
        tempToken.mColumn = -1 ;

        resultTree->aToken = tempToken ;
        resultTree->isEmpty = false ;
        resultTree->left = NULL ;
        resultTree->right = NULL ;
      } // else
    } // else
  } // else

  return resultTree ;
} // EvalStringOpr()

TreeNodePtr EvalEq( TreeNodePtr inSExpTree, string evalType ) {
  TreeNodePtr resultTree = new TreeNode ;
  vector<Token> printErrorList ;

  // check number of arguments
  if ( inSExpTree->right->left == NULL ) {
    resultTree = NULL ;
    
    gerrorEvalMsg = 5 ;
    cout << "ERROR (incorrect number of arguments) : " << inSExpTree->left->aToken.content << endl ;
    PrintTree( inSExpTree->left, NULL, printErrorList ) ;
    throw ( printErrorList ) ;
  } // if()
  else if ( inSExpTree->right->right->left == NULL ) {
    resultTree = NULL ;
    
    gerrorEvalMsg = 5 ;
    cout << "ERROR (incorrect number of arguments) : " << inSExpTree->left->aToken.content << endl ;
    PrintTree( inSExpTree->left, NULL, printErrorList ) ;
    throw ( printErrorList ) ;
  } // if
  else if ( inSExpTree->right->right->right->left != NULL ) {
    resultTree = NULL ;
    
    gerrorEvalMsg = 5 ;
    cout << "ERROR (incorrect number of arguments) : " << inSExpTree->left->aToken.content << endl ;
    PrintTree( inSExpTree->left, NULL, printErrorList ) ;
    throw ( printErrorList ) ;
  } // else if()
  else {
    TreeNodePtr tempTree1 = new TreeNode ;
    TreeNodePtr tempTree2 = new TreeNode ;
    bool answer = false ;

    tempTree1 = EvalSExp( inSExpTree->right->left ) ;
    tempTree2 = EvalSExp( inSExpTree->right->right->left ) ;

    if ( evalType == "eqv?" ) {
      if ( IsAtom( tempTree1->aToken ) == true && tempTree1->aToken.type != STRING ) { // tree1 is Atom
        if ( IsAtom( tempTree2->aToken ) == true && tempTree2->aToken.type != STRING ) { // tree2 is Atom
          if ( IsSameTree( tempTree1, tempTree2 ) == true )
            answer = true ;
          else
            answer = false ;
        } // if()
        else
          answer = false ;
      } // if()
      else { // NotATom
        if ( tempTree1 == tempTree2 )
          answer = true ;
        else
          answer = false ;
      } // else
    } // if()
    else { // "equal"
      answer = IsSameTree( tempTree1, tempTree2 ) ;
    } // else

    // make resultTree
    if ( answer == true ) {
      Token tempToken ;
      tempToken.content = "#t" ;
      tempToken.type = T ;
      tempToken.mLine = -1 ;
      tempToken.mColumn = -1 ;

      resultTree->aToken = tempToken ;
      resultTree->isEmpty = false ;
      resultTree->left = NULL ;
      resultTree->right = NULL ;
    } // if()
    else {
      Token tempToken ;
      tempToken.content = "nil" ;
      tempToken.type = NIL ;
      tempToken.mLine = -1 ;
      tempToken.mColumn = -1 ;

      resultTree->aToken = tempToken ;
      resultTree->isEmpty = false ;
      resultTree->left = NULL ;
      resultTree->right = NULL ;
    } // else
  } // else

  return resultTree ;
} // EvalEq()

bool IsSameTree( TreeNodePtr tree1, TreeNodePtr tree2 ) {
  if ( tree1 == NULL && tree2 == NULL )
    return true ;
  else if ( tree1 == NULL && tree2 != NULL )
    return false ;
  else if ( tree1 != NULL && tree2 == NULL )
    return false ;
  else
    return ( tree1->aToken.content == tree2->aToken.content ) 
           && IsSameTree( tree1->left, tree2->left ) && IsSameTree( tree1->right, tree2->right ) ;
} // IsSameTree()

TreeNodePtr EvalClean( TreeNodePtr inSExpTree ) {
  TreeNodePtr resultTree = new TreeNode ;
  vector<Token> printErrorList ;

  // check number of arguments
  if ( inSExpTree->right->aToken.type != NIL ) {
    
    gerrorEvalMsg = 5 ;
    cout << "ERROR (incorrect number of arguments) : " << inSExpTree->left->aToken.content << endl ;
    PrintTree( inSExpTree->left, NULL, printErrorList ) ;
    throw ( printErrorList ) ;
  } // if()
  else {
    gVariable.clear() ;
    cout << "environment cleaned" << endl ;
  } // else()

  resultTree = NULL ;
  return resultTree ;
} // EvalClean()

TreeNodePtr EvalExit( TreeNodePtr inSExpTree ) {
  TreeNodePtr resultTree = new TreeNode ;
  vector<Token> printErrorList ;

  // check number of arguments
  if ( inSExpTree->right->aToken.type != NIL ) {
    
    gerrorEvalMsg = 5 ;
    cout << "ERROR (incorrect number of arguments) : " << inSExpTree->left->aToken.content << endl ;
    PrintTree( inSExpTree->left, NULL, printErrorList ) ;
    throw ( printErrorList ) ;
  } // if()
  else {
    gexitMsg = true ;
  } // else()

  resultTree = NULL ;
  return resultTree ;
} // EvalExit()

TreeNodePtr EvalBegin( TreeNodePtr inSExpTree ) {
  TreeNodePtr resultTree = new TreeNode ;
  vector<Token> printErrorList ;

  // check number of arguments
  if ( inSExpTree->right->left == NULL ) {
    
    gerrorEvalMsg = 5 ;
    cout << "ERROR (incorrect number of arguments) : " << inSExpTree->left->aToken.content << endl ;
    PrintTree( inSExpTree->left, NULL, printErrorList ) ;
    throw ( printErrorList ) ;
  } // if()
  else {
    TreeNodePtr tempTree = new TreeNode ;
    tempTree = inSExpTree->right ;

    while ( tempTree->right->left != NULL ) {
      resultTree = EvalSExp( tempTree->left ) ;

      tempTree = tempTree->right ;
    } // while()

    resultTree = EvalSExp( tempTree->left ) ;
  } // else()

  return resultTree ;
} // EvalBegin()

TreeNodePtr EvalIf( TreeNodePtr inSExpTree ) {
  TreeNodePtr resultTree = new TreeNode ;
  vector<Token> printErrorList ;

  // check number of arguments
  if ( inSExpTree->right->left == NULL ) {
    resultTree = NULL ;
    
    gerrorEvalMsg = 5 ;
    cout << "ERROR (incorrect number of arguments) : " << inSExpTree->left->aToken.content << endl ;
    PrintTree( inSExpTree->left, NULL, printErrorList ) ;
    throw ( printErrorList ) ;
  } // if()
  else if ( inSExpTree->right->right->left == NULL ) {
    resultTree = NULL ;
    
    gerrorEvalMsg = 5 ;
    cout << "ERROR (incorrect number of arguments) : " << inSExpTree->left->aToken.content << endl ;
    PrintTree( inSExpTree->left, NULL, printErrorList ) ;
    throw ( printErrorList ) ;
  } // if
  else if ( inSExpTree->right->right->right->left == NULL ) { // argNum == 2 no else's return
    TreeNodePtr tempTree = new TreeNode ;
    tempTree = EvalSExp( inSExpTree->right->left ) ;

    if ( tempTree->aToken.content != "nil" && tempTree->aToken.content != "0" )
      resultTree = EvalSExp( inSExpTree->right->right->left ) ;
    else {
      gerrorEvalMsg = 8 ;
      PrintTree( inSExpTree, NULL, printErrorList ) ;
      throw ( printErrorList ) ;
    } // else
  } // else if()
  else if ( inSExpTree->right->right->right->right->aToken.type == NIL ) { // argNum == 3
    TreeNodePtr tempTree = new TreeNode ;
    tempTree = EvalSExp( inSExpTree->right->left ) ;

    if ( tempTree->aToken.content != "nil" && tempTree->aToken.content != "0" )
      resultTree = EvalSExp( inSExpTree->right->right->left ) ;
    else 
      resultTree = EvalSExp( inSExpTree->right->right->right->left ) ;
  } // else if()
  else {
    resultTree = NULL ;
    
    gerrorEvalMsg = 5 ;
    cout << "ERROR (incorrect number of arguments) : " << inSExpTree->left->aToken.content << endl ;
    PrintTree( inSExpTree->left, NULL, printErrorList ) ;
    throw ( printErrorList ) ;
  } // else 

  return resultTree ;
} // EvalIf()

TreeNodePtr EvalCond( TreeNodePtr inSExpTree ) {
  TreeNodePtr resultTree = new TreeNode ;
  vector<Token> printErrorList ;
  int condNum = 0 ;

  TreeNodePtr checkListTree = new TreeNode ;
  checkListTree = inSExpTree->right ;

  while ( checkListTree->left != NULL ) {

    if ( checkListTree->left->aToken.content != "" ) {
      resultTree = NULL ;

      gerrorEvalMsg = 9 ;
      PrintTree( inSExpTree, NULL, printErrorList ) ;
      throw ( printErrorList ) ;

      return resultTree ;
    } // if()
    
    if ( checkListTree->left->right->aToken.type == NIL ) {
      resultTree = NULL ;

      gerrorEvalMsg = 9 ;
      PrintTree( inSExpTree, NULL, printErrorList ) ;
      throw ( printErrorList ) ;

      return resultTree ;
    } // if()

    checkListTree = checkListTree->right ;
  } // while()

  if ( inSExpTree->right->left == NULL ) { // (cond)
    resultTree = NULL ;

    gerrorEvalMsg = 9 ;
    PrintTree( inSExpTree, NULL, printErrorList ) ;
    throw ( printErrorList ) ;

    return resultTree ;
  } // if()
  else {
    TreeNodePtr walkTree = new TreeNode ;
    walkTree = inSExpTree->right ;

    while ( walkTree->right->left != NULL ) { // while have another cond.
      condNum = condNum + 1 ;

      if ( walkTree->left->isEmpty == false ) {
        resultTree = NULL ;

        gerrorEvalMsg = 9 ;
        PrintTree( inSExpTree, NULL, printErrorList ) ;
        throw ( printErrorList ) ;

        return resultTree ;
      } // if()
      else if ( walkTree->left->right->left == NULL ) {
        resultTree = NULL ;

        gerrorEvalMsg = 9 ;
        PrintTree( inSExpTree, NULL, printErrorList ) ;
        throw ( printErrorList ) ;

        return resultTree ;
      } // if()
      else {
        TreeNodePtr tempTree = new TreeNode ;
        tempTree = EvalSExp( walkTree->left->left ) ;

        if ( tempTree->aToken.type != NIL ) {
          TreeNodePtr tempTree2 = new TreeNode ;
          tempTree2 = walkTree->left->right ;

          // check list
          while ( tempTree2->left != NULL ) 
            tempTree2 = tempTree2->right ;

          if ( tempTree2->aToken.type != NIL ) {
            resultTree = NULL ;

            gerrorEvalMsg = 9 ;
            PrintTree( inSExpTree, NULL, printErrorList ) ;
            throw ( printErrorList ) ;

            return resultTree ;
          } // if() 

          tempTree2 = walkTree->left->right ;

          while ( tempTree2->left != NULL ) {
            resultTree = EvalSExp( tempTree2->left ) ;

            tempTree2 = tempTree2->right ;
          } // while()
          
          return resultTree ;
        } // if()
      } // else
      
      walkTree = walkTree->right ;
    } // while()

    // check else
    if ( walkTree->left->isEmpty == false ) {
      resultTree = NULL ;

      gerrorEvalMsg = 9 ;
      PrintTree( inSExpTree, NULL, printErrorList ) ;
      throw ( printErrorList ) ;

      return resultTree ;
    } // if()
    else if ( walkTree->left->right->left == NULL ) {
      resultTree = NULL ;

      gerrorEvalMsg = 9 ;
      PrintTree( inSExpTree, NULL, printErrorList ) ;
      throw ( printErrorList ) ;

      return resultTree ;
    } // if()
    else {

      if ( condNum != 0 && ( walkTree->left->left->aToken.content == "else" ) ) {
        TreeNodePtr tempTree2 = new TreeNode ;
        tempTree2 = walkTree->left->right ;

        // check list
        while ( tempTree2->left != NULL ) 
          tempTree2 = tempTree2->right ;

        if ( tempTree2->aToken.type != NIL ) {
          resultTree = NULL ;

          gerrorEvalMsg = 9 ;
          PrintTree( inSExpTree, NULL, printErrorList ) ;
          throw ( printErrorList ) ;

          return resultTree ;
        } // if() 

        tempTree2 = walkTree->left->right ;

        while ( tempTree2->left != NULL ) {
          resultTree = EvalSExp( tempTree2->left ) ;

          tempTree2 = tempTree2->right ;
        } // while()
          
        return resultTree ;
      } // if ()
      else {
        TreeNodePtr tempTree = new TreeNode ;
        tempTree = EvalSExp( walkTree->left->left ) ;

        if ( tempTree->aToken.type != NIL ) {
          TreeNodePtr tempTree2 = new TreeNode ;
          tempTree2 = walkTree->left->right ;

          // check list
          while ( tempTree2->left != NULL ) 
            tempTree2 = tempTree2->right ;

          if ( tempTree2->aToken.type != NIL ) {
            resultTree = NULL ;

            gerrorEvalMsg = 9 ;
            PrintTree( inSExpTree, NULL, printErrorList ) ;
            throw ( printErrorList ) ;

            return resultTree ;
          } // if() 

          tempTree2 = walkTree->left->right ;

          while ( tempTree2->left != NULL ) {
            resultTree = EvalSExp( tempTree2->left ) ;

            tempTree2 = tempTree2->right ;
          } // while()

          return resultTree ;
        } // if()
        else {
          resultTree = NULL ;

          gerrorEvalMsg = 8 ;
          PrintTree( inSExpTree, NULL, printErrorList ) ;
          throw ( printErrorList ) ;
      
          return resultTree ;
        } // else
      } // else
    } // else
  } // else

  return resultTree ;
} // EvalCond()