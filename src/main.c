#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * Starting point of the program, write anything here
 */
const char* reg(char var) {
    switch (var) {
        case 'a': return "eax";
        case 'b': return "ebx";
        case 'c': return "ecx";
        case 'd': return "edx";
        default: return NULL;
    }
}

void transpile_line(const char *line) {
    const char *p = line;
    while (*p == ' ' || *p == '\t') p++;
    if (*p == '\0' || *p == '\n') return;
    char dest = *p++;
    while (*p && (*p == ' ' || *p == '\t')) p++;
    if (*p != '=') return;
    p++;
    while (*p && (*p == ' ' || *p == '\t')) p++;

    char op1[32] = {0}, op2[32] = {0};
    char oper = 0;
    int i = 0;
    while (*p && !strchr("+-*/; \t", *p)) op1[i++] = *p++;
    op1[i] = '\0';
    while (*p == ' ' || *p == '\t') p++;
    if (*p == '+' || *p == '-') {
        oper = *p++;
        while (*p == ' ' || *p == '\t') p++;
        i = 0;
        while (*p && *p != ';' && *p != '\n') op2[i++] = *p++;
        op2[i] = '\0';
    }

    // After parsing op1 and before translation, detect mult/div/logic/shifts
    // Detect operator if not + or -
    if (!oper) {
        // check for other operators before parsing op2
        if (*p == '&' || *p == '|' || *p == '^') {
            oper = *p++;
            while (*p == ' ' || *p == '\t') p++;
            i = 0;
            while (*p && *p != ';' && *p != '\n') op2[i++] = *p++;
            op2[i] = '\0';
        } else if (*p == '<' && *(p+1) == '<') {
            oper = 'L'; // shift left
            p += 2;
            while (*p == ' ' || *p == '\t') p++;
            i = 0;
            while (*p && *p != ';' && *p != '\n') op2[i++] = *p++;
            op2[i] = '\0';
        } else if (*p == '>' && *(p+1) == '>') {
            oper = 'R'; // shift right
            p += 2;
            while (*p == ' ' || *p == '\t') p++;
            i = 0;
            while (*p && *p != ';' && *p != '\n') op2[i++] = *p++;
            op2[i] = '\0';
        } else if (*p == '*' || *p == '/') {
            oper = *p++;
            while (*p == ' ' || *p == '\t') p++;
            i = 0;
            while (*p && *p != ';' && *p != '\n') op2[i++] = *p++;
            op2[i] = '\0';
        }
    }

    const char *dest_reg = reg(dest);
    if (!oper) {
        if (isdigit(op1[0]))
            printf("MOV %s, %s\n", dest_reg, op1);
        else {
            const char *src = reg(op1[0]);
            printf("MOV %s, %s\n", dest_reg, src);
        }
    } else {
        // Generate code for binary ops: add/sub, logic, shifts, mul/div
        const char *src2 = isdigit(op2[0]) ? op2 : reg(op2[0]);
        switch (oper) {
            case '+': printf("ADD %s, %s\n", dest_reg, src2); break;
            case '-': printf("SUB %s, %s\n", dest_reg, src2); break;
            case '&': printf("AND %s, %s\n", dest_reg, src2); break;
            case '|': printf("OR %s, %s\n", dest_reg, src2); break;
            case '^': printf("XOR %s, %s\n", dest_reg, src2); break;
            case 'L': printf("SHL %s, %s\n", dest_reg, src2); break;
            case 'R': printf("SHR %s, %s\n", dest_reg, src2); break;
            case '*':
                // multiplication: use EAX*src2
                // if multiplying a (eax), EAX already holds A so just MUL
                if (dest_reg[2] == 'x' && dest_reg[1] == 'a') {
                    printf("MUL %s\n", src2);
                } else {
                    printf("MOV eax, %s\n", dest_reg);
                    printf("MUL %s\n", src2);
                    printf("MOV %s, eax\n", dest_reg);
                }
                break;
            case '/':
                // division: handle literal zero first
                if (isdigit(op2[0]) && atoi(op2) == 0) {
                    printf("Error\n");
                    break;
                }
                printf("MOV eax, %s\n", dest_reg);
                printf("DIV %s\n", src2);
                printf("MOV %s, eax\n", dest_reg);
                break;
        }
    }
}

int main() {
    char line[256];
    char inner[256];
     while (fgets(line, sizeof(line), stdin)) {
         const char *p = line;
         while (*p == ' ' || *p == '\t') p++;
         if (strncmp(p, "for", 3) == 0) {
             // parse for(init; cond; incr)
            char init[128] = {0}, cond_s[128] = {0}, incr_s[128] = {0};
            const char *q = p + 3; int ii = 0, ci = 0, ri = 0;
            while (*q && *q != '(')
                q++;
            if (*q == '(')
                q++;
            while (*q && *q != ';') init[ii++] = *q++;
            init[ii] = '\0'; if (*q == ';') q++;
            while (*q && *q != ';') cond_s[ci++] = *q++;
            cond_s[ci] = '\0'; if (*q == ';') q++;
            while (*q && *q != ')') incr_s[ri++] = *q++;
            incr_s[ri] = '\0';
            // init
            transpile_line(init);
            // start loop label
            printf("start_loop:\n");
            // condition
            // parse condition: trim whitespace, extract var, comp, rhs
            char *t = cond_s;
            while (*t && isspace(*t)) t++;
            char var = *t++;
            while (*t && isspace(*t)) t++;
            char comp[3] = {0};
            if ((t[0]=='>'||t[0]=='<'||t[0]=='='||t[0]=='!') && t[1]=='=') {
                comp[0] = t[0]; comp[1] = t[1]; t += 2;
            } else {
                comp[0] = *t++;
            }
            comp[2] = '\0';
            while (*t && isspace(*t)) t++;
            char rhs[32] = {0}; int rj = 0;
            if (isdigit(*t)) {
                while (*t && isdigit(*t)) rhs[rj++] = *t++;
            } else if (isalpha(*t)) {
                rhs[rj++] = *t++;
            }
            rhs[rj] = '\0';
            // CMP and jump false
            const char *r1 = reg(var);
            const char *r2 = isdigit(rhs[0]) ? rhs : reg(rhs[0]);
            printf("CMP %s, %s\n", r1, r2);
            const char *jmpf = "JMP";
            if (strcmp(comp, "==")==0) jmpf = "JNE";
            else if (strcmp(comp, "!=")==0) jmpf = "JE";
            else if (strcmp(comp, ">=")==0) jmpf = "JL";
            else if (strcmp(comp, ">")==0) jmpf = "JLE";
            else if (strcmp(comp, "<=")==0) jmpf = "JG";
            else if (strcmp(comp, "<")==0) jmpf = "JGE";
            printf("%s end_label\n", jmpf);
            // block
            while (fgets(inner, sizeof(inner), stdin)) {
                const char *r = inner; while (*r==' '||*r=='\t') r++;
                if (*r == '}') break;
                transpile_line(inner);
            }
            // increment
            // handle var++ or var-- or var = var +/- value
            if (strstr(incr_s, "++")) {
                printf("ADD %s, 1\n", reg(incr_s[0]));
            } else if (strstr(incr_s, "--")) {
                printf("SUB %s, 1\n", reg(incr_s[0]));
            } else {
                // fallback to transpile assignment
                transpile_line(incr_s);
            }
            printf("JMP start_loop\n");
            printf("end_loop:\n");
        } else if (strncmp(p, "while", 5) == 0) {
            // parse while(cond)
            const char *q = p + 5;
            while (*q && *q != '(') q++;
            if (*q == '(') q++;
            const char *r2 = q;
            while (isspace(*r2)) r2++;
            char var = *r2++;
            while (isspace(*r2)) r2++;
            char comp[3] = {0};
            if ((r2[0]=='>'||r2[0]=='<'||r2[0]=='='||r2[0]=='!') && r2[1]=='=') {
                comp[0]=r2[0]; comp[1]=r2[1]; r2+=2;
            } else {
                comp[0]=*r2++;
            }
            comp[2] = '\0';
            while (isspace(*r2)) r2++;
            char rhs_str[32] = {0}; int ri = 0;
            if (isdigit(*r2)) {
                while (isdigit(*r2)) rhs_str[ri++] = *r2++;
            } else if (isalpha(*r2)) {
                rhs_str[ri++] = *r2++;
            }
            rhs_str[ri] = '\0';
            // start label
            printf("start_loop:\n");
            // compare and jump false
            const char *rgt = isdigit(rhs_str[0]) ? rhs_str : reg(rhs_str[0]);
            printf("CMP %s, %s\n", reg(var), rgt);
            const char *jmpf = "JMP";
            if (strcmp(comp, "==")==0) jmpf = "JNE";
            else if (strcmp(comp, "!=")==0) jmpf = "JE";
            else if (strcmp(comp, ">=")==0) jmpf = "JL";
            else if (strcmp(comp, ">")==0) jmpf = "JLE";
            else if (strcmp(comp, "<=")==0) jmpf = "JG";
            else if (strcmp(comp, "<")==0) jmpf = "JGE";
            printf("%s end_label\n", jmpf);
             // block
             while (fgets(inner, sizeof(inner), stdin)) {
                 const char *r = inner; while (*r==' '||*r=='\t') r++;
                 if (*r=='}') break;
                 transpile_line(inner);
             }
            printf("JMP start_loop\n");
            printf("end_label:\n");
        } else if (strncmp(p, "if", 2) == 0) {
             // parse condition of form if (v op rhs) {
            char var = 0, comp[3] = {0}, rhs[32] = {0};
            const char *q = p + 2; int ci = 0, ri = 0;
            while (*q && *q != '(') q++;
            if (*q == '(') q++;
            while (*q == ' ' || *q == '\t') q++;
            var = *q++;
            while (*q == ' ' || *q == '\t') q++;
            // parse comparator
            if ((q[0] == '>' || q[0] == '<' || q[0] == '=' || q[0] == '!') && q[1] == '=') {
                comp[ci++] = *q++;
                comp[ci++] = *q++;
            } else {
                comp[ci++] = *q++;
            }
            comp[ci] = '\0';
            while (*q == ' ' || *q == '\t') q++;
            while (*q && *q != ')' && *q != ' ' && *q != '\t') rhs[ri++] = *q++;
            rhs[ri] = '\0';
            // emit compare
            const char *r1 = reg(var);
            const char *r2 = isdigit(rhs[0]) ? rhs : reg(rhs[0]);
            printf("CMP %s, %s\n", r1, r2);
            // select jump if false
            const char *jmp = "JMP";
            if (strcmp(comp, "==") == 0) jmp = "JNE";
            else if (strcmp(comp, "!=") == 0) jmp = "JE";
            else if (strcmp(comp, ">=") == 0) jmp = "JL";
            else if (strcmp(comp, ">") == 0) jmp = "JLE";
            else if (strcmp(comp, "<=") == 0) jmp = "JG";
            else if (strcmp(comp, "<") == 0) jmp = "JGE";
            printf("%s end_label\n", jmp);
            // process block
            while (fgets(inner, sizeof(inner), stdin)) {
                const char *r = inner;
                while (*r == ' ' || *r == '\t') r++;
                if (*r == '}') break;
                transpile_line(inner);
            }
            printf("end_label:\n");
        } else {
            transpile_line(line);
        }
    }
    return 0;
}