# include <stdio.h>
# include <iostream>
# include <string>
# include <string.h>
# include <vector>
# include <exception>

using namespace std ;

int gTestNum = 0 ;
bool gexitMsg = false ;
bool geof = false ;
int gerrorMsg = 0 ;
int gLine = 1 ; // because first char is '\n'
int gColumn = 0 ;

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

char GetNextChar() ;
char PeekNextChar() ;
void GetToken( Token &nextToken ) ;
bool IsWhiteSpace( char ch ) ;
bool IsDigit( char ch ) ;
bool IsLetter( char ch ) ;
float StringToFloat( string inputString ) ;
void SkipComment() ;
void SkipErrorLine() ;
char SkipWhiteSpace() ;
bool IsAtom( Token aToken ) ;
void ReadSExp( Token aToken, vector<Token> &sExpList ) ;
void GetSExpForTree( vector<Token> sExpList, int &i, vector<Token> &sExpForTree ) ;
TreeNodePtr BuildTree( vector<Token> sExpList, int &i ) ;
void PrintTree( TreeNodePtr sExpTree, TreeNodePtr parentNode, vector<Token> &printList ) ;
void PrintRightTree( TreeNodePtr sExpTree, vector<Token> &printList ) ;
void PrettyPrint( vector<Token> printList ) ;


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
        
          PrintTree( sExpTree, NULL, printList ) ;
          PrettyPrint( printList ) ;

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
          cout << endl << "> ";
          hasCoutArr = true ;
        } // if()

      } // catch
    } // if()

    gColumn = 0 ;

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
    

    if ( gexitMsg == false && geof == false && syntaxToken.size() > 0 && hasCoutArr == false ) {
      cout << endl << "> ";
    } // if()

    gerrorMsg = 0 ;
    gLine = 1 ;
    syntaxToken.clear() ;
    syntaxForTree.clear() ;
    printList.clear() ;
    i = 0 ;

  } // while()

  if ( gexitMsg == false && geof == true ) // END-OF-FILE encountered and NOT 「user entered '(exit)'」
    cout << "ERROR (no more input) : END-OF-FILE encountered" ;

  cout << endl << "Thanks for using OurScheme!" ;
} // main()

// ---------------------------- Functions' Body  ----------------------------

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
      if ( sExpList.size() == 2 && sExpList[0].content == "(" && sExpList[1].content == "exit" ) {
        sExpList.push_back( aToken ) ;
        gexitMsg = true ;
      } // if()
      else {
        // give it ". nil"
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
      } // else
    } // else if()
    else {
      gerrorMsg = 2 ;
      throw ( aToken ) ;
    } // else

    if ( sExpList.size() >= 5 && sExpList[0].content == "(" && sExpList[1].content == "exit" ) {
      int size = sExpList.size() ;
      bool isExit = true ;

      for ( int ii = 2 ; ii < size ; ii ++ ) {
        if ( sExpList[ii].content != "." && sExpList[ii].content != "nil" 
             && sExpList[ii].content != "(" && sExpList[ii].content != ")" )

          isExit = false ;
      } // for()

      if ( isExit == true )
        gexitMsg = true ;
    } // if()
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
    
    tempToken.content = ")" ;
    tempToken.type = RIGHTPAREN ;
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

    sExpTree->right = BuildTree( sExpForTree, i ) ;

    i = i + 1 ; // read ')'

    return sExpTree ;
  } // else
} // BuildTree()

void PrintTree( TreeNodePtr sExpTree, TreeNodePtr parentNode, vector<Token> &printList ) {
  if ( sExpTree != NULL ) {
    Token tempToken ;
    
    if ( sExpTree->isEmpty == true ) {
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
      if ( parentNode->right->isEmpty == false && parentNode->right->aToken.type != NIL ) {
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

    // for QUOTE
    if ( sExpTree->aToken.type == RIGHTPAREN && sExpTree->isEmpty == true )
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

} // PrettyPrint()