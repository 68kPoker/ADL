
/* adlcomp.c prototypes */

void main(int argc, char *argv[]);
void getadlargs(int argc, char *argv[]);
void print_usage(char *cname);
char *getnext(char *argv[], int *indx);
void init();
void breaker();
void adlcomp();
void printmsg();
void getfile();
void mkpath(char *s, char *dir, char *base);
void check_predefs();
void checkmax(int n, int max_n, char *name);
void write_code();
void adlstats();
void writebuf(int f, void *b, struct adldir *d);
void writestuff(int ifile, int ofile, struct adldir *dir);
void wrapup();

/* adllex.c prototypes */

void lexer();
int gettoken(char *s);
int eatwhite();
int getstring(char *s);
int getescape();
int isnumber(char *s);
int adlident(char *s);
int mygetc();
int myunget(int c);
void emit_file();
int open_input(char *name);
void close_input();
void save_input(char **buffer);
void restore_input(char *buffer);

/* adlmisc.c prototypes */

void get_local();
void unget_local();
void getvars();
void getlist(int16 t);
int16 getassign(int do_insert);
address adlrout(int16 t_read);
void getverb();
void globassign();
void routassign();
void prepassign();
void getverbsyn(int16 verb);

/* adlmsg.c prototypes */

void eatuntil(int16 c);
void error(char *s);
void warning(char *s);
void fatal(char *s);

/* adlobj.c prototypes */

int16 noun_exists(int16 adj, int16 noun);
void move_obj(int16 obj, int16 dest);
int16 new_noun(int16 mod, int16 noun);
int16 getnew(int16 t_read);
int16 getold(int16 t_read, int16 t_save);
void setprop(int16 obj, int16 which, int16 val);
void nounassign(int16 t_read, int16 t_save);
void getnouns();

/* codegen.c prototypes */

address oldcode(address addr, char opcode, int16 opnd);
address newcode(char opcode, int16 opnd);
address currcode();

/* compdict.c prototypes */

int16 lookup(char *s, int16 *val);
int16 insertkey(char *s, int16 type, int16 val, int16 first);
int16 insert_sys(char *s, int16 type, int16 val);
int16 insert_local(char *s, int16 type, int16 val);
void count_symtab( int debug );
void write_symtab(int fd, int debug);
void del_locals();

/* predef.c prototypes */

void init_predefs();

/* routcomp.c prototypes */

void getglob();
void getpair();
void getexpr( int16 t_read );
void getform();
void getwhile();
void getif();
int16 getargs();
void getroutine(int16 t_read);

/* mytime.c prototypes */

void checkbreak(int (*rout)());

