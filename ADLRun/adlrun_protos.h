
/* adlarith.c prototypes */

int16 myrand( int16 n );
void do_div();
void do_mod();
void do_and();
void do_or();

/* adldwim.c prototypes */

int16 dwimmer( int16 rp, int16 obj );
void dwimerror( int16 n );
int16 dwimadj( int16 which, int16 adj );
int16 dwimnoun( int16 which, int16 noun );
int dwimi();
int dwimd();

/* adlerr.c prototypes */

void error( int num );

/* adlfuncs.c prototypes */

void dosysfunc( int16 rp );
void assertargs( char *s, int16 n );
void do_rtrn();
void do_exit();
void do_val();
void getstring( char *s );
int16 yesno();
void do_args();

/* adlintrn.c prototypes */

void setdemon();
void deldemon();
void setfuse();
void delfuse( int16 act, int16 which );
void incturn();
void retturn();
void doprompt();
void setactor();
void delactor( int16 n );

/* adlmach.c */

void callrouts();
void execdems();
void execfuses();
void runprog();
void push( int16 x );
int16 pop();
void doret();
void breaker();
void docall( int16 stackdepth );
void printstack();
void callone( int16 rp );
void u_prompt();

/* adlmacro.c */

void define( char *str1, char *str2 );
int16 nummacro();
void undef( char *str );
char *expand( char *str );
void clearmacro();

/* adlmiscr.c prototypes */

void varconts( int16 n );
void setg();
void saystr();

/* adlobjr.c prototypes */

void setp();
void move_obj();
void objprop( int16 n );
void vset();
void vprop();

/* adlrun.c prototypes */

void main( int argc, char *argv[] );
void getadlargs( int argc, char *argv[] );
void init( int16 first );
void driver();
char *getnext( int *which, char *argv[] );
void rand_seed();
void sayer( char *s );
void loadarray( char **a, struct adldir *d, int16 first );
void getchunk( char **s, char *word );
void crlf();
int get_buffer();
int16 noun_exists( int16 adj, int16 noun );

/* adlscrn.c prototypes */

void head_setup();
int mygetstr( char *which, char *where, int need );
void write_head();
void head_term();

/* adlspec.c prototypes */

void special();
void restart();
void set_tty();
void change_tty();
void invert( char *s, int16 n );
void savegame( char *savename );
int restoregame( char *savename );
void do_wordfile();
void callextern();
void scripter();

/* adlstr.c prototypes */

void eqstring();
void substring();
void lengstring();
void readstring();
void catstring();
void chrstring();
void ordstring();
int16 strpos( char *s1, char *s2 );
void posstring();
void savestr();
void do_str();
void do_num();
char *xlate( char *s );
void do_name();
void do_vname();
void do_mname();
void do_pname();
void do_define();
void do_undef();

/* adltrans.c prototypes */

void setverb();
void hitverb();
void missverb();

/* rtdict.c prototypes */

int16 insertkey( char *name, int16 type, int16 val, int16 first );
int16 lookup( char *name, int16 *val );
char *findone( int16 type, int16 val );
void read_symtab( int fd, struct adldir *dir );

/* rtlex.c prototypes */

void gettoken();
int lexer();
int try_expand();
int numberp( char *s );
void getquotes( int16 ch );
void eatwhite();
int adlchr( int c );

/* rtparse.c prototypes */

int Find_PP( Prep1, Mod, Noun, Prep2 );
int Find_VP( int16 verb, int16 prep );
void Perform_Action( int which );
void initvars();
int16 parse();
