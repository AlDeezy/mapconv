#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int args_call_help(int argc, char **argv);
void print_help(char **argv);

int main(int argc, char **argv) {
  if (argc < 2 || args_call_help(argc, argv)) {
    print_help(argv);
    return 0;
  }

  FILE *input_file = fopen(argv[1], "r");
  if (input_file == NULL) {
    fprintf(stderr, "Error opening file: %s\n", argv[1]);
  }

  const char *output_filename = "output.map";
  FILE *output_file = fopen(output_filename, "w+");
  if (input_file == NULL) {
    fprintf(stderr, "Error creating file: %s\n", output_filename);
  }

  int unnamed_vars = 1;
  char *line = NULL;
  size_t line_len = 0;
  ssize_t read;
  const int element_size = 128;
  const int max_elements = 6;
  char first_element[element_size];
  sprintf(first_element, "balls");
  char line_elements[max_elements][element_size];

  while ((read = getline(&line, &line_len, input_file)) != -1) {
    // init line vars
    int element_counter = 0;
    char line_copy[line_len];
    strcpy(line_copy, line);

    // strip newline
    char *pch = strstr(line_copy, "\n");
    if (pch != NULL)
      strncpy(pch, "\0", 1);

    // token init and loop
    char *token = strtok(line_copy, "\t");
    if (token != NULL) {
      if (strcmp(token, first_element) == 0) {
        // duplicate entry
        continue;
      }
      sprintf(first_element, "%s", token);
    }
    while (token != NULL && element_counter < max_elements) {
      strcpy(line_elements[element_counter], token);
      token = strtok(NULL, "\t");
      element_counter++;
    }

    // Sanitization
    if (element_counter == max_elements) {
      for (int i = 0; i < strlen(line_elements[element_counter - 1]); i++) {
        if (isspace(line_elements[element_counter - 1][i])) {
          line_elements[element_counter - 1][i] = '_';
        } else {
          line_elements[element_counter - 1][i] =
              tolower(line_elements[element_counter - 1][i]);
        }
      }
    } else if (element_counter == max_elements - 1) {
      char varname[element_size];
      sprintf(varname, "unnanmed-var-%i", unnamed_vars);
      unnamed_vars++;
      strcpy(line_elements[5], varname);
      element_counter++;
    }

    // Writing to file
    if (element_counter == max_elements) {
      fprintf(output_file, "80%s %s\n", line_elements[0],
              line_elements[element_counter - 1]);
      fprintf(stdout, "80%s %s\n", line_elements[0],
              line_elements[element_counter - 1]);
    }
  }

  fclose(input_file);
  fclose(output_file);
  return 0;
}

int args_call_help(int argc, char **argv) {
  for (int i = 0; i < argc; i++) {
    if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
      return 1;
    }
  }
  return 0;
}

void print_help(char **argv) {
  fprintf(stdout,
          "USAGE:\n%s [FLAGS]... INPUT-FILES...\n\n"
          "FLAGS:\n"
          "-h, --help: Print this dialogue\n"
          "INPUT-FILES:\n"
          "Valid files are BizHawk .wch files.\n"
          "Inputting one will create the other.\n",
          argv[0]);
}
