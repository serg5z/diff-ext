#include <sys/types.h>
#include <rxposix.h>
#include <stdio.h>
#include <stdlib.h>
#include <io.h>

/********************************************************************
*command line args:                                                 *
*  --input  - input file name. if '-' is specified, or unspecified  *
*             standart input is used.                               *
*  --output - output file name. if '-' is specified, or unspecified *
*             standart output is used.                              *
*  --help   - prints out usage help.                                *
********************************************************************/

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
  "\t\t\tstandart input is used.\n"
  "\t--help\t\tprints out usage help.\n";

  printf(message, program_name, program_name);
}

void
update_version(FILE* input, FILE* output) {
  char buffer[4096];
  char* pattern ="[[:space:]]*FILEVERSION"
    "[[:space:]]*\\([[:digit:]]*\\)[[:space:]]*,"
    "[[:space:]]*\\([[:digit:]]*\\)[[:space:]]*,"
    "[[:space:]]*\\([[:digit:]]*\\)[[:space:]]*,"
    "[[:space:]]*\\([[:digit:]]*\\)[[:space:]]*";
  int ret;
  regex_t re;

  if ((ret=regcomp(&re, pattern, REG_ICASE )) != 0) {
    regerror(ret, &re, buffer, sizeof(buffer));
    fprintf(stderr, "inc_build: %s (%s)\n", buffer, pattern);
  }
  else {
    while(fgets(buffer, 4096, input)) {
      regmatch_t version[5];
      if((ret = regexec(&re, buffer, 5, version, 0)) == 0) {
	char tmp[8];
	int major;
	int minor;
	int patch;
	int build;
	
	if(version[1].rm_so > 0) {
	  strncpy(tmp, buffer+version[1].rm_so, min(version[1].rm_eo-version[1].rm_so, 8));
	  major = atoi(tmp);
	}
	if(version[2].rm_so > 0) {
	  strncpy(tmp, buffer+version[2].rm_so, min(version[2].rm_eo-version[2].rm_so, 8));
	  minor = atoi(tmp);
	}
	if(version[3].rm_so > 0) {
	  strncpy(tmp, buffer+version[3].rm_so, min(version[3].rm_eo-version[3].rm_so, 8));
	  patch = atoi(tmp);
	}
	if(version[4].rm_so > 0) {
	  strncpy(tmp, buffer+version[4].rm_so, min(version[4].rm_eo-version[4].rm_so, 8));
	  build = atoi(tmp);
	}
	
	build++;
	
	fprintf(output, "FILEVERSION %d, %d, %d, %d\n", major, minor, patch, build);
      } else {
	fputs(buffer, output);
      }
    }
    
    regfree(&re);
  }
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
      }
      else {
        fprintf(stderr, "Unknown option '%s'\n\n", argv[index]);
        usage(argv[0]);
        index = argc;
	input_name = 0;
	output_name = 0;
	input = 0;
	output = 0;
      }
    }
  }
  
  if((input != 0) && (output != 0)) {    
    if((strcmp(input_name, output_name) == 0) && (strcmp(input_name, "-") != 0)) {
      char template_name[] = "iv_XXXXXX";
      char* tmp_name = mktemp(template_name);
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
