#include <sys/types.h>
#include <io.h>
#include <rxposix.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/********************************************************************
*command line args:                                                 *
*  --input  - input file name. if '-' is specified, or unspecified  *
*             standart input is used.                               *
*  --output - output file name. if '-' is specified, or unspecified *
*             standart output is used.                              *
*  --help   - prints out usage help.                                *
********************************************************************/
typedef int (UPDATE_FUNCTION*)(int);

int
increment(int x) {
  return x+1;
}

int 
decrement(int x) {
  return x-1;
}

int
set(int x) {
}

int 
min(int x1, int x2) {
  int ret = x1;
  
  if(x2 < x1) {
    ret = x2;
  }
  
  return ret;
}

void
usage(char* program_name) {
  char* message = 
  "%s usage:\n"
  "%s [options]\n"
  "\n"
  "Options:\n"
  "\t--input\t\tinput file name. if '-' is specified, or unspecified\n"
  "\t\t\tstandart input is used.\n"
  "\t--output\toutput file name. if '-' is specified, or unspecified\n"
  "\t\t\tstandart output is used.\n"
  "\t--help\t\tprints out usage help.\n";

  printf(message, program_name, program_name);
}

int
init(char* update_string, update_function[4] update) {
  int result = -1;
/*  char* update_re_str = "\\([[:digit:]]+|\\+|\\*\\)\\.\\([[:digit:]]+|\\+|\\*\\)\\.\\([[:digit:]]+|\\+|\\*\\)\\.\\([[:digit:]]+|\\+|\\*\\)";*/
  char* update_re_str = 
    "\\([[:digit:]][[:digit:]]*\\|\\*\\|\\+\\|-\\)\\."
    "\\([[:digit:]][[:digit:]]*\\|\\*\\|\\+\\|-\\)\\."
    "\\([[:digit:]][[:digit:]]*\\|\\*\\|\\+\\|-\\)\\."
    "\\([[:digit:]][[:digit:]]*\\|\\*\\|\\+\\|-\\)";
  regex_t update_re;
  regmatch_t version[5];
  char buffer[4096];
  int ret;
  
  memset(&update_re, 0, sizeof(regex_t));
  
  if((ret = regcomp(&update_re, update_re_str, REG_ICASE)) == 0) {
    if(regexec(&update_re, update_string, 5, version, 0) == 0) {
      int n;
      
      if(version[1].rm_so >= 0) { 
        n = min(version[1].rm_eo-version[1].rm_so, 4096);
        strncpy(buffer, update_string+version[1].rm_so, n);
        buffer[n] = '\0';
        printf("1: '%s'\n", buffer);
      }
      if(version[2].rm_so > 0) { 
        n = min(version[2].rm_eo-version[2].rm_so, 4096);
        strncpy(buffer, update_string+version[2].rm_so, n);
        buffer[n] = '\0';
        printf("2: '%s'\n", buffer);
      }
      if(version[3].rm_so > 0) { 
        n = min(version[3].rm_eo-version[3].rm_so, 4096);
        strncpy(buffer, update_string+version[3].rm_so, n);
        buffer[n] = '\0';
        printf("3: '%s'\n", buffer);
      }
      if(version[4].rm_so > 0) { 
        n = min(version[4].rm_eo-version[4].rm_so, 4096);
        strncpy(buffer, update_string+version[4].rm_so, n);
        buffer[n] = '\0';
        printf("4: '%s'\n", buffer);
      }
      result = 0;
    }
    regfree(&update_re);
  } else {
    regerror(ret, &update_re, buffer, sizeof(buffer)/sizeof(buffer[0]));
    fprintf(stderr, "update_version: %s (%s)\n", buffer, update_re_str);
  }

  return result;
}

void
update_version(FILE* input, FILE* output) {
  char buffer[4096];
  char* fileversion_re_str =
    /* 1 */ "\\(^[[:space:]]*FILEVERSION[[:space:]]*\\)"
    /* 2 */ "\\([[:digit:]][[:digit:]]*\\)"
    /* 3 */ "\\([[:space:]]*,[[:space:]]*\\)"
    /* 4 */ "\\([[:digit:]][[:digit:]]*\\)"
    /* 5 */ "\\([[:space:]]*,[[:space:]]*\\)"
    /* 6 */ "\\([[:digit:]][[:digit:]]*\\)"
    /* 7 */ "\\([[:space:]]*,[[:space:]]*\\)"
    /* 8 */ "\\([[:digit:]][[:digit:]]*\\)"
    /* 9 */ "\\([[:space:]]*$\\)";
  char* productversion_re_str =
    /* 1 */ "\\(^[[:space:]]*PRODUCTVERSION[[:space:]]*\\)"
    /* 2 */ "\\([[:digit:]][[:digit:]]*\\)"
    /* 3 */ "\\([[:space:]]*,[[:space:]]*\\)"
    /* 4 */ "\\([[:digit:]][[:digit:]]*\\)"
    /* 5 */ "\\([[:space:]]*,[[:space:]]*\\)"
    /* 6 */ "\\([[:digit:]][[:digit:]]*\\)"
    /* 7 */ "\\([[:space:]]*,[[:space:]]*\\)"
    /* 8 */ "\\([[:digit:]][[:digit:]]*\\)"
    /* 9 */ "\\([[:space:]]*$\\)";
  int ret;
  regex_t fileversion_re;
  regex_t productversion_re;
  int re_compiled = 1;

  memset(&fileversion_re, 0, sizeof(regex_t));
  memset(&productversion_re, 0, sizeof(regex_t));

  if((ret=regcomp(&fileversion_re, fileversion_re_str, REG_ICASE)) != 0) {
    regerror(ret, &fileversion_re, buffer, sizeof(buffer)/sizeof(buffer[0]));
    fprintf(stderr, "update_version: %s (%s)\n", buffer, fileversion_re_str);
    re_compiled = 0;
  }
  
  if((ret=regcomp(&productversion_re, productversion_re_str, REG_ICASE)) != 0) {
    regerror(ret, &fileversion_re, buffer, sizeof(buffer)/sizeof(buffer[0]));
    fprintf(stderr, "update_version: %s (%s)\n", buffer, productversion_re_str);
    re_compiled = 0;
  }
  
  if(re_compiled) {
    int major = -1;
    int minor = -1;
    int patch = -1;
    int build = -1;
    
    while(fgets(buffer, 4096, input)) {
      regmatch_t version[10];
      if((regexec(&fileversion_re, buffer, 10, version, 0) == 0) || 
         (regexec(&productversion_re, buffer, 10, version, 0) == 0)) {
        char tmp[8] = "";
        char space[5][4096] = {"", "", "", "", ""};
        int new_build;
      
        if(version[1].rm_so >= 0) { /* may be 0 for the first one */
          strncpy(space[0], buffer+version[1].rm_so, min(version[1].rm_eo-version[1].rm_so, 4096));
        }
        if(version[2].rm_so > 0) {
          memset(tmp, 0, sizeof(tmp)/sizeof(tmp[0]));
          strncpy(tmp, buffer+version[2].rm_so, min(version[2].rm_eo-version[2].rm_so, 8));
          major = atoi(tmp);
        }
        if(version[3].rm_so > 0) {
          strncpy(space[1], buffer+version[3].rm_so, min(version[3].rm_eo-version[3].rm_so, 4096));
        }
        if(version[4].rm_so > 0) {
          memset(tmp, 0, sizeof(tmp)/sizeof(tmp[0]));
          strncpy(tmp, buffer+version[4].rm_so, min(version[4].rm_eo-version[4].rm_so, 8));
          minor = atoi(tmp);
        }
        if(version[5].rm_so > 0) {
          strncpy(space[2], buffer+version[5].rm_so, min(version[5].rm_eo-version[5].rm_so, 4096));
        }
        if(version[6].rm_so > 0) {
          memset(tmp, 0, sizeof(tmp)/sizeof(tmp[0]));
          strncpy(tmp, buffer+version[6].rm_so, min(version[6].rm_eo-version[6].rm_so, 8));
          patch = atoi(tmp);
        }
        if(version[7].rm_so > 0) {
          strncpy(space[3], buffer+version[7].rm_so, min(version[7].rm_eo-version[7].rm_so, 4096));
        }
        if(version[8].rm_so > 0) {
          memset(tmp, 0, sizeof(tmp)/sizeof(tmp[0]));
          strncpy(tmp, buffer+version[8].rm_so, min(version[8].rm_eo-version[8].rm_so, 8));
          build = atoi(tmp);
        }
        if(version[9].rm_so > 0) {
          strncpy(space[4], buffer+version[9].rm_so, min(version[9].rm_eo-version[9].rm_so, 4096));
        }
        
        new_build = build+1;
        
        fprintf(output, "%s%d%s%d%s%d%s%d%s", space[0], major, space[1], minor, space[2], patch, space[3], new_build, space[4]);
      } else {
        fputs(buffer, output);
      }
    }
  }
  
  regfree(&fileversion_re);
  regfree(&productversion_re);
}

int
main(int argc, char** argv) {
  FILE* input = stdin;
  FILE* output = stdout;
  char* input_name = "-";
  char* output_name = "-";
  int status = 0;
  int remove_input = 0;
  
  if(argc > 1) {
    int index = 1;
    while(index < argc) {
      if(strcmp(argv[index], "--input") == 0) {
        index++;
        input_name = argv[index];
        index++;
        
        if(input_name == 0) {
          usage(argv[0]);
          index = argc;
        }
      } else if(strcmp(argv[index], "--output") == 0) {
        index++;
        output_name = argv[index];
        index++;
        if(output_name == 0) {
          usage(argv[0]);
          index = argc;
        }
      } else if(strcmp(argv[index], "--help") == 0) {
        usage(argv[0]);
        index = argc;
        output_name = 0;
        input_name = 0;
        input = 0;
        output = 0;
      } else {
        if(init(argv[index]) != 0) {
          fprintf(stderr, "Unknown option '%s'\n\n", argv[index]);
          usage(argv[0]);
          index = argc;
          input_name = 0;
          output_name = 0;
          input = 0;
          output = 0;
        } else {
          return 0;
        }
      }
    }
  }
  
  if((input != 0) && (output != 0)) {    
    if((strcmp(input_name, output_name) == 0) && (strcmp(input_name, "-") != 0)) {
/*      char template_name[] = "iv_XXXXXX"; // was used with mktemp() */
      char* tmp_name = tmpnam(0);
      int ret;
      
      ret = rename(input_name, tmp_name);
      if(ret != 0) {
        perror("rename");
      }
      else {
        remove_input = 1;
        
        input = fopen(tmp_name, "r");
        if(input == 0) {
          perror("fopen(...,\"r\")");
          fprintf(stderr, "Can not open '%s' for reading.\n", tmp_name);
        }
        else {
          output = fopen(output_name, "w");
          
          if(output == 0) {
            perror(argv[0]);
            fprintf(stderr, "Can not open '%s' for writing.\n", output_name);
            rename(tmp_name, input_name);
          }
          
          input_name = tmp_name;
        }
      }
    } else {
      if(strcmp(output_name, "-") != 0) {
        output = fopen(output_name, "w");
        
        if(output == 0) {
          fprintf(stderr, "Can not open '%s' for writing.\n", output_name);
        }
      }
      
      if(strcmp(input_name, "-") != 0) {
        input = fopen(input_name, "r");
  
        if(input == 0) {
          fprintf(stderr, "Can not open '%s' for reading.\n", input_name);
        }
      }
    }
    
   update_version(input, output);
    
    if(input != stdin) {
      fclose(input);
    }
    
    if(output != stdout) {
      fflush(output);
      fclose(output);
    }
    
    if(remove_input) {
      int ret;
      
      ret = remove(input_name);
      
      if(ret != 0) {
        perror("Can not remove tmp file");
      }
    }
  } else {
    status = 1;
  }
  
  return status;
}
