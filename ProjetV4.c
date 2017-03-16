#define _POSIX_SOURCE
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#define NB_FICHIER 10

//On crée une structure pour les processus enfants
typedef struct child_worker_s {
    unsigned cw_id;
    pid_t cw_pid;
    int cw_pipe[2];
} child_worker_t;

int descriptionProcessus(const char *whoami)
{
        printf("Je suis le processus %s.  Mon pid est :%d  Mon ppid est :%d\n", whoami, getpid(), getppid() );
        return 1;
}

int main(void)
{
	//Déclaration des variables
	//n est le nombre de processus enfant que l'on souhaite créer
        int n= 10;
        int i=0;
        char j='0';
        unsigned long long entier;
        unsigned long long diviseur=0;
        unsigned long long nombreEncours;
        //char messageEcrire[8];
        unsigned id;
        //unsigned entier;
        child_worker_t child_workers[10];
    
        //declaration des fichiers
        FILE* fichier_pere = NULL;
    	FILE* fichier_fils[NB_FICHIER];
    	
    	//allocation de la memeoire
    //	fichier_fils = (FILE*)malloc(sizeof(FILE)*NB_FICHIER);
    	//Initialisation à Null des fichier
    	for(i=0;i<NB_FICHIER;++i){
    		fichier_fils[i]=NULL;
    	}
    	
    	//ouveture du fichier pere
        fichier_pere = fopen("primes-to-100k.txt", "r");
    
        //declarations des variables
    	int nb[3];
    	int stop=0;
    	unsigned long long nombreEntre;
    	unsigned long long nombreEnCours;
    	
    	//valeur max du fichier 
    	unsigned long long valmax=15485863;
    	
    	//on recupere la borne max de l'intervalle globale
    	printf("Entrez votre valeur inferieur a %llu\n",valmax);
    	scanf("%llu",&nombreEntre);
    	
    	//calcul de l'intervalle
    	unsigned long long intervalle = (nombreEntre/NB_FICHIER)+1;
    	printf("intervalle: %llu\n",intervalle);
    	unsigned long long somme_intervalle=intervalle;
    	//creation du nom du premier fils Primer_0.txt
    	int nCmpt=0;
    	char sCmpt='0';
    	char nom_fichier[]="Primer_x.txt";
    	nom_fichier[7]=sCmpt;
    	
    	
     	if (fichier_pere != NULL)
        {
            // On peut lire et écrire dans le fichier
            printf("fichier non null\n");
            
            //on commence par le premier fichier
            fichier_fils[nCmpt]=fopen(nom_fichier,"w");
            
            //on lit tous les nombres du fichier jusqu'a la borne max ou la fin du fichier
            do
    	    {
    	        //printf("Numfichier %d\n",nCmpt);

    	    	fscanf (fichier_pere, "%llu", &nombreEnCours); 
    	    	//printf("nombreEnCours: %llu\n",nombreEnCours);
    	    	//si le nombre en cours et superieur aà l'intervalle 
    	    	//on change de fichier
    	    	if(nombreEnCours<=nombreEntre){
    		    	if(nombreEnCours > somme_intervalle){
    //		    		printf("----------------------------------%llu\n", nombreEnCours);
    		    		//on ferme le fichier
    		    		fclose(fichier_fils[nCmpt]);
    		    		//on incremente les compteurs pour passer au fichier suivant 
    		    		sCmpt++;
    		    		nCmpt++;
    		    		nom_fichier[7]=sCmpt;
    		    		fichier_fils[nCmpt]=fopen(nom_fichier,"w");
    		    		//on augmente l'intervalle 
    		    		somme_intervalle+=intervalle;
    		   		}
    		   		//on ecrit dans le fichier fils
    		   		fprintf(fichier_fils[nCmpt], "%llu\n", nombreEnCours);
    //		   		printf( "%llu\n", nombreEnCours);
    //				system("pause");
    	   		}
    	   		
    	    }while (!feof(fichier_pere) && nombreEnCours<=nombreEntre );
    	    
    	    //fermeture du fichier pere 
    		fclose(fichier_pere);
    		fclose(fichier_fils[nCmpt]);
    
        }
        else
        {
            // On affiche un message d'erreur si on veut
            printf("Impossible d'ouvrir le fichier test.txt");
        }
    
        printf("On crée %d enfants\n", n);
        descriptionProcessus("parent");
        
        int NB_FILS=nCmpt+1;
        for(j='0',i=0;i<NB_FILS;i++,j++){
            nom_fichier[7]=j;
            fichier_fils[i]=fopen(nom_fichier,"r");
        }
        //Création des n processus enfant
        for(i=0,id=0;i<NB_FILS;i++,id++)
        {
            child_workers[id].cw_id = id;
            //printf("ID de l'enfant : %d\n",child_workers[id].cw_id);
            //Si le tube n'est pas crée
            if(pipe(child_workers[id].cw_pipe)!=0)
            {
                printf("Erreur de création du tube.\n");
            }
            pid_t pid=fork();
            child_workers[id].cw_pid=pid;
            //Si le processus n'existe pas
            if(pid<0)
            {
                printf("Erreur de création du processus");
            }

            //Code du processus fils
            if (pid==0) /*On exécute cette partie du code seulement s'il s'agit du processus enfant */
            {
                printf("---------- %d----------------\n",id);
                if (fichier_fils[i] != NULL && nombreEntre>=(i*intervalle) && diviseur==0 )
                {
                    printf("\tetape lecture du fichier numero %d\n",i);
                    // On peut lire et Ècrire dans le fichier
                    while(fscanf(fichier_fils[i],"%llu\n",&nombreEncours) != EOF && diviseur==0)
                    {
                        //printf("Nombre encours :%llu",nombreEncours);
                        //printf("%llu / %llu = %llu",nombreEntre,nombreEncours,(nombreEntre%nombreEncours));
                        if (nombreEntre%nombreEncours == 0)
                        {
                            close(child_workers[id].cw_pipe[0]);
                            if(nombreEntre==nombreEncours)
                            {
                                printf("\t\t%llu est un nombre premier\n",nombreEntre);
                                entier = nombreEncours;
                            }
                            else
                            {
                                diviseur=nombreEncours;
                                entier = diviseur;
                                printf("\t\t diviseur %llu\n",diviseur);
                                printf("\t\t%llu est divisible par %llu\n",nombreEntre,diviseur);
                                
                            }
                            write(child_workers[id].cw_pipe[1], &entier, sizeof(unsigned long long));
                            
                        }

                    }
                }
                else
                {
                    printf("\tnon lecture du fichier numero %d\n",i);
                    close(child_workers[id].cw_pipe[0]);
                    write(child_workers[id].cw_pipe[1], 0, sizeof(unsigned long long));
                }
                exit(0);
            }
		
            /*Le processus père attend que les processus enfants se terminent*/

          if(pid>0) {
                 wait(NULL);
            	entier=0;
            	close(child_workers[i].cw_pipe[1]);
                //sprintf(messageEcrire, "Bonjour");
                read(child_workers[i].cw_pipe[0], &entier,sizeof(unsigned long long));
                
            	if(entier != 0)
            	{
            	    if(nombreEntre==entier){
            	        printf("%llu est un nombre premier\n",entier);
                         
                    }
                    if(nombreEntre!=entier){
                         printf("Le diviseur est %llu\n",entier);
                      }
            	    int j;
                    for(j=0;j<=NB_FILS;j++){
                            kill(child_workers[j].cw_pid,SIGKILL);
                            child_workers[j].cw_pid=0;
                    }
                }
            }
                    
        	
    	
        }

        //supression des fichiers
        for(i=0;i<NB_FILS;i++,j++)
        {
            nom_fichier[7]=j;
            remove(nom_fichier);
        }
        
    
        
	return 0;
}

