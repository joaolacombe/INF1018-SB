    
    /* João Pedro Garcia 1211768 3WB */
    /* Matheus Caldas 1312760 3WB */
	
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>


    typedef int (*funcp) (int a, int b, int c, int d, int e);
    
    int verifica_var_par_cte(char *s);
    int verifica_operacao(char *s);
    int verifica_indice(char *s);
    FILE * f_retorno(FILE *arq, unsigned char *cod, int *pos_cod);
    funcp geracod(FILE *f);




	   FILE * f_ifseq(FILE *arq, unsigned char *cod, int *pos_cod, int *vje, int *vje_linha, int *cont_je, int *vjpos_preencher){
        /*funcao que trata o caso de desvio*/

        /*cod é nosso vetor com os codigos. pos_cod é a posição deste vetor

        vje é o vetor que guarda o endereço da instrucao após o jump 
        vje_linha é o vetor que guarda as linhas que queremos ir do jump
        vje_pos_preencher é o vetor que guarda a posição do vetor cod onde iremos botar o endereço do jump depois
        cont_je é o contador desses destes 3 vetores

        op1 guarda o primeirando operando. op2 o segundo..
        liha_lida a linha que queremos'ir*/
         char op1[10], op2[10], linha_lida_str[5];
         int linha_lida;
         int ind1,vpc1,ind2,vpc2;
         int pos = *pos_cod;

         
         fscanf(arq, "%s", op1);
         fscanf(arq,"%s",op2);
         fscanf(arq,"%s",linha_lida_str);
                    
         linha_lida = (int) atoi(linha_lida_str);
         printf("linha lida foi %d", linha_lida);
         ind1 = verifica_indice(op1);
   		 ind2 = verifica_indice(op2);
    	 vpc1 = verifica_var_par_cte(op1);
   		 vpc2 = verifica_var_par_cte(op2);
			
         
         /*Botando os valores nas variaveis/parametros/constantes, aqui vou preenchendo o vetor codigo*/
     


            /*Botando o primeiro operador em %ecx*/
            if(vpc1 == 2)//tenho um parametro
            {
                /*movl ind1*8(%ebp) %ecx*/
                    cod[pos] = 0x8b;
                    pos++;
                    cod[pos] = 0x4d;
                    pos++;
                    cod[pos] = 8 + 4*ind1;
                    pos++;
            }
     
     
            else if(vpc1 == 1)//tenho uma variavel
            {
                 /* fazendo um movl ind1*-4(%ebp) %ecx*/
                    cod[pos] = 0x8b;
                    pos++;
                    cod[pos] = 0x4d;
                    pos++;
                    cod[pos] = 252 - 4*ind1;
                    pos++;
            }
            else if(vpc1==3) //tenho uma constante
            {       

                    /*movl $ind1, %ecx*/
                    /* fazendo movl pra %ecx*/
                    cod[pos] = 0xb9;
                    pos++;

                    /*guradando o valor */
                    cod[pos] = (unsigned char) ind1;
                    pos++;
                    cod[pos] = (unsigned char) (ind1>>8);
                    pos++;
                    cod[pos] = (unsigned char) (ind1>>16);
                    pos++;
                    cod[pos]= (unsigned char) (ind1>>24);
                    pos++;
     
            }
             
            /*Botando o segundo operador em %edx*/
            if(vpc2 == 2){ 
                    //tenho um parametro
                    //movl ind2*8(ebp), %edx
                    cod[pos] = 0x8b;
                    pos++;
                    cod[pos] = 0x55;
                    pos++;
     
                    cod[pos] = 8 + 4*ind2;
                    pos++;
            }
            else if(vpc2==1){
                    //tenho uma variavel


                    /* fazendo um movl ind2*-4(%ebp) pra %edx*/
                    cod[pos] = 0x8b;
                    pos++;
                    cod[pos] = 0x55;
                    pos++;
                    cod[pos] = 252 - 4*ind2;
                    pos++;
     
     
            }
            else if(vpc2==3){ 
                    /*tenho uma CTE
                    movl $ind2, %edx


                    fazendo movl pra %edx*/
                    cod[pos] = 0xba;
                    pos++;

                    /*guradando o valor */
                    cod[pos] = (unsigned char) ind2;
                    pos++;
                    cod[pos] = (unsigned char) (ind2>>8);
                    pos++;
                    cod[pos] = (unsigned char) (ind2>>16);
                    pos++;
                    cod[pos]= (unsigned char) (ind2>>24);
                    pos++;
            }
         
         //preenchendo com o codigo de cmp %ecx com %edx que é 39 CA
         cod[pos] = 0x39;
         pos++;
         cod[pos] = 0xca;
         pos++;
         
         vjpos_preencher[*cont_je] = pos + 2; 
         /* vje_pos_preencher é o vetor que guarda a posição do vetor cod,
         onde iremos botar o endereço do jump depois*/
        
         vje[*cont_je] = (int)&cod[pos + 6]; //vje recebendo o endereço da instrucao após o jump

         vje_linha[*cont_je] = linha_lida; //vje_linha recebendo a linha que o jump quer ir


         *cont_je = *cont_je + 1;
         
         /*Preenchendo com o codigo de je*/
         cod[pos] = 0x0f;
         pos++;
         cod[pos] = 0x84;

         pos+=5;
         /*+5 pq teremos os 4 bytes vazios que tera o endereço para onde queremos ir*/ 
     		
         
         *pos_cod = pos;
         return arq;
       }


    FILE * f_retorno(FILE *arq, unsigned char *cod, int *pos_cod){
        /*funcao que trata o caso retorno*/

        /*cod é o vetor com os codigos e pos_cod sua posicao
        
        ret tipo é se o retorno é de uma variavel, parametro ou constante*
        ret indice é o valor de 0 a 4 se for variavel ou parametro ou o valor da constante
        temp2 apenas le de forma temporaria o elemento que queremos retornar
        */

        int ret_ind, ret_tipo;
        int pos = *pos_cod;
        char temp2[3];

        fscanf(arq, "%s", temp2);
        ret_ind = verifica_indice(temp2);
        ret_tipo = verifica_var_par_cte(temp2);
        
        if(ret_tipo==1){
            /*retorno de uma variavel, botando no %eax*/
             cod[pos] = 0x8b;
             pos++;
             cod[pos] = 0x45;
             pos++;
             cod[pos] = 252 - 4*ret_ind;
             pos++;

        }
        else if(ret_tipo == 2){
            /*retorno de um parametro, botando no %eax*/
             cod[pos] = 0x8b;
             pos++;
             cod[pos] = 0x45;
             pos++;
             cod[pos] = 8 + 4*ret_ind;
             pos++;

        }
        else if(ret_tipo == 3){
            /*retorno de uma constante, botando no %eax*/
            cod[pos] = 0xb8;
            pos++;
            cod[pos] = (unsigned char) ret_ind;
            pos++;
            cod[pos] = (unsigned char) (ret_ind>>8);
            pos++;
            cod[pos] = (unsigned char) (ret_ind>>16);
            pos++;
            cod[pos]= (unsigned char) (ret_ind>>24);
            pos++;

        }


        /* obs: o comando ret e os movls finais estão na função geracod*/

        *pos_cod = pos;

        return arq;
    }
    FILE * f_operacao(FILE *arq, char *temp, unsigned char *cod, int *pos_cod){

            /*funcao que realiza as operacoes entre variaveis, constantes e parametros*/

            /*arq é o arquivo que recebemos*/
            /*temp possui a variavel/parametro que vai receber a operacao*/
            /*retornamos o arquivo para a funcao chamadora

            op1 é o primeiro operando. op2 o segundo. op o símbolo da nossa operação(+,-,*)
            operacao define o tipo de operacao que estamos fazendo

            vpc1/2 e res_vpc tem um inteiro que diz se é parametro, variavel ou constante
            ind1/2 e res_ind tem um inteiro que diz qual variavel ou parametro ou no caso de constante qual o seu valor
            */
            int pos;
            char op1[10], op2[10];
            char op[5];
            char temp2[3];
            int operacao;
            int vpc1,vpc2,ind1,ind2;
            int res_ind, res_vpc;
     		
	
     
            pos = *pos_cod;
            fscanf(arq, "%s", temp2);//salvei o := em temp
            fscanf(arq, "%s", op1);//op1 tem primeiro operando
            fscanf(arq, "%s", op); //op tem a operacao
            fscanf(arq, "%s", op2); //op2 tem segundo operando
  
            ind1 = verifica_indice(op1);
            ind2 = verifica_indice(op2);
            vpc1 = verifica_var_par_cte(op1);
            vpc2 = verifica_var_par_cte(op2);
            operacao = verifica_operacao(op);
		
            res_ind = verifica_indice(temp);
            res_vpc = verifica_var_par_cte(temp);
   		
            
            /*Botando os valores nas variaveis/parametros/constantes, aqui devo ir preenchendo o vetor codigo*/
            
            /*primeiro operando ponho em %ecx*/
            if(vpc1 == 2)//tenho um parametro
            {
                    //movl ind1*8(%ebp), %ecx
                    cod[pos] = 0x8b;
                    pos++;
                    cod[pos] = 0x4d;
                    pos++;
                    cod[pos] = 8 + 4*ind1;
                    pos++;
            }
     
     
            else if(vpc1 == 1)//tenho uma variavel
            {
                    //movl ind1*-4(%ebp), %ecx
                    cod[pos] = 0x8b;
                    pos++;
                    cod[pos] = 0x4d;
                    pos++;
                    cod[pos] = 252 - 4*ind1;
                    pos++;
            }
            else if(vpc1==3) {

                    /* fazendo movl pra %ecx*/
                    cod[pos] = 0xb9;
                    pos++;

                    /*guardando o valor */
                    cod[pos] = (unsigned char) ind1;
                    pos++;
                    cod[pos] = (unsigned char) (ind1>>8);
                    pos++;
                    cod[pos] = (unsigned char) (ind1>>16);
                    pos++;
                    cod[pos]= (unsigned char) (ind1>>24);
                    pos++;
     
            }
            
     
            /*botando o segundo operando em %edx*/
            if(vpc2 == 2){ //tenho um parametro
                    
                    //movl ind2*8(ebp), %edx
                    cod[pos] = 0x8b;
                    pos++;
                    cod[pos] = 0x55;
                    pos++;
     
                    cod[pos] = 8 + 4*ind2;
                    pos++;
            }
            else if(vpc2==1){ //tenho uma variavel
                    /* fazendo um movl ind2*-4(%ebp) %edx*/
                    cod[pos] = 0x8b;
                    pos++;
                    cod[pos] = 0x55;
                    pos++;

                    cod[pos] = 252 - 4*ind2;
                    pos++;
     
     
            }
            else if(vpc2==3){ //tenho uma CTE
                    //movl $ind2, %edx

                   /* fazendo movl pra %edx*/
                    cod[pos] = 0xba;
                    pos++;

                    /*guradando o valor */
                    cod[pos] = (unsigned char) ind2;
                    pos++;
                    cod[pos] = (unsigned char) (ind2>>8);
                    pos++;
                    cod[pos] = (unsigned char) (ind2>>16);
                    pos++;
                    cod[pos]= (unsigned char) (ind2>>24);
                    pos++;
            }
     
            
            if(operacao == 1){
                    //soma
     
                    cod[pos] = 0x01;
                    pos++;
                    cod[pos] = 0xd1;
                    pos++;
     
                    //addl %edx, %ecx   adicionando os valores
            }
            else if(operacao == 2){
                    //subtracao
                    cod[pos] = 0x29;
                    pos++;
                    cod[pos] = 0xd1;
                    pos++;
     
                    //subl %edx, %ecx   subtranindo os valores
                }
            else if(operacao == 3){
                    //multiplicacao
     
                    cod[pos] = 0x0f;
                    pos++;
                    cod[pos] = 0xaf;
                    pos++;
                    cod[pos] = 0xca;
                    pos++;
     
                    //imul %edx, %ecx  multiplicando os valores
            }
     
           if(res_vpc == 2)//tenho um parametro
            {
                    //movl %ecx, res_ind*8(%ebp)      
                    cod[pos] = 0x89;
                    pos++;
                    cod[pos] = 0x4d;
                    pos++;
                    cod[pos] = 8 + 4*res_ind;
                    pos++;
     
     
            }
            else if(res_vpc == 1){//tenho uma variavel
                    //movl %ecx, res_ind*-4(%ebp)
     
                    cod[pos] = 0x89;
                    pos++;
                    cod[pos] = 0x4d;
                    pos++;
                    cod[pos] = 252 - 4*res_ind;
                    pos++;
     
     
            }
     
            *pos_cod = pos;
     
            return arq;
    }

    int verifica_var_par_cte(char *s){
            /*retorna 1 para variavel e 2 para parametro e 3 para constantes*/

            char varpar[4];

            /*Copiando para testar se temos uma variavel ou parametro*/
            strncpy(varpar,s,1);

            varpar[1]='\0';//precisei adicionar manualmente
     			
            if( strcmp(varpar,"v") == 0 ){                  
			return 1;
                    /* Caso onde temos uma variável */
            }

            else if( strcmp (varpar, "p") == 0){   
                    return 2;
                    /*Caso onde temos um parametro*/
            }
     
            else if( strcmp (varpar, "$") == 0 )
            { 
                    return 3;
                    //constante
     
            }	

	return 0; //caso do programa estar escrito errado, como não foi necessário, não tratamos
    }
    int verifica_operacao(char *s){
            /*funcao que apenas verifica qual operador em questao.*/
            /*retorna 1 para soma, 2 para subtração e 3 para multiplicação*/

            if( strcmp (s,"+") == 0)
                    return 1;
            else
                    if( strcmp(s,"-") == 0)
                            return 2;
                    else
                            if( strcmp (s,"*") == 0)
                                    return 3; 

            return 0;
    }
    int verifica_indice(char *s){
            /*s é a palavra podendo ser v0,v1,v2,v3,v4 assim como p0,p1,p2,p3,p4 ou a constante $numero*/
            /* a funcao retorna o valor inteiro do indice podendo ser 0,1,2,3 ou 4 ou o valor da cte*/
        return atoi(s+1);
    }
    funcp geracod(FILE *f){ 

        /*
        vje é o vetor que guarda o endereço da instrucao após o jump 
        vje_linha é o vetor que guarda as linhas que queremos ir do jump
        vje_pos_preencher é o vetor que guarda a posição do vetor cod onde iremos botar o endereço do jump depois
        cont_je é o contador desses destes 3 vetores

        linhas é o vetor que armazena o endereço das linhas do arquivo lido
        */
            char temp[10];
            unsigned char *cod = malloc(sizeof(char)*500);
            int j,k, endereco, pos_mudanca;
            int i=0,cont_je=0,pos_cod=0;
      		int vje_linha[500],vje[500],vjpos_preencher[500],linhas[500];
            
              
            
            /*push %ebp
            mov %esp, %ebp
            */
            cod[pos_cod] = 0x55;
            pos_cod++;
            cod[pos_cod] = 0x89;
            pos_cod++;
            cod[pos_cod] = 0xe5;
            pos_cod++;
            
            /*sub 20 %esp, alocando espaco para as variaveis*/
            cod[pos_cod] = 0x83;
            pos_cod++;
            cod[pos_cod] = 0xec;
            pos_cod++;
            cod[pos_cod] = 0x14;
            pos_cod++;
     
            while(fscanf(f,"%s", temp) == 1){

                    /*Armazenando o endereço de linhas do arquivo texto*/   
                    linhas[i] = (int) &cod[pos_cod];
     
                    if( strcmp(temp, "ret") == 0 ){
                            //caso retorno
                            f = f_retorno(f,cod, &pos_cod);
                    }
     
                    else
                            if( strcmp(temp, "ifeq") == 0){
     					          //caso condicional, avaliar por ultimo no trabalho
                                   f = f_ifseq(f,cod,&pos_cod,vje,vje_linha,&cont_je, vjpos_preencher);
                      		}
                            else
                            {
                                    //teremos uma variavel, parametro ou constantes e, portanto, atribuicao
                                   
                                    f = f_operacao(f,temp,cod, &pos_cod); 
                            }
                     
                     i++;
              
            }

      		for(k=0;k<cont_je;k++)
            {
                /*for para preencher os endereços dos jumps */
            	
                /*endereço recebe endereço da linha que queremos ir menos o endereço da instrução pós jmp*/
                endereco = linhas [ vje_linha[k] - 1]   -    vje[k]; 

                /*pos_mudanca recebe a posicao do vetor codigo que vamos botar o endereço*/   
                pos_mudanca = vjpos_preencher[k];

                /*preenchendo o vetor codigo com o endereo */
                cod[pos_mudanca] = (unsigned char) endereco;
          		pos_mudanca++;
                cod[pos_mudanca] = (unsigned char) (endereco>>8);
                pos_mudanca++;
                cod[pos_mudanca] = (unsigned char) (endereco>>16);
                pos_mudanca++;
                cod[pos_mudanca]= (unsigned char) (endereco>>24);
                pos_mudanca++;
            }
            /*Finalizando, movl do esp pro ebp, pop do ebp e ret*/
            
            cod[pos_cod] = 0x89;
            pos_cod++;
            cod[pos_cod] = 0xec;
            pos_cod++;
            cod[pos_cod] = 0x5d;
            pos_cod++;
            cod[pos_cod] = 0xc3;
            pos_cod++;
            /*Finalizando*/

           

          
        /*for apenas pra visualizar o vetor codigo preenchido*/
         for(j=0;j<pos_cod;j++)
            {
                   if(j==(pos_cod-4))
                          printf("\n\n");
                   printf("--%x", cod[j]);
     
         }


            printf("\n\n");
            return (funcp)cod;
    }
    int main (void){

            int k;
            funcp funcao;

          
            FILE *arq = fopen("teste.txt", "r");
     
            if(arq==NULL){
                    printf("Erro de abertura do arquivo");
                    exit(1);
            }
            
            funcao = geracod(arq);
            k=(*funcao)(3,1,2,-3,4);
            printf("valor resultado : %d\n\n", k);
            return 0;
    }




