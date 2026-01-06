#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <linux/limits.h>
#include <limits.h>
#include <sys/types.h>
#include <dirent.h>
#include <openssl/sha.h>
#include <pwd.h>

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

typedef struct blob_t
{
	char hash[41];
	char dir_path[18];
	char file_path[57];
	int size;
	char* content;
} blob_t;

typedef struct tree_t
{
	char hash[41];
	char dir_path[18];
	char file_path[57];
	int size;
	char* content;
} tree_t;

typedef struct save_t
{
	char hash[41];
	char dir_path[18];
	char file_path[57];
	int size;
	char* content;
} save_t;

/* Global değişken */
struct stat st = {0};

/* Fonksiyon Prototipleri */
void PrintHelp();
void PrintVersion();
void PrintHint();
void Init();
void Status();
void Track(int argc, char** argv);
void Save(char* save_message);
void UnTrack(int argc, char** argv);
void SetConfig(int argc, char** argv);
void SetDefaultBranchName(char* default_branch_name);
void GenerateBranch(int argc, char** argv);
void Merge(char* to_branch, char* merge_save_message);
void DeleteBranch(int argc, char** argv);
void ListBranches();
void Checkout(char* branch_or_save_hash);
void AmendSave(char* new_save_message);
void Reflog();
void Log();
void Log_OneLine();
void ListClean();
void RemoveClean();
void AddLog(int argc, char** argv);
void calculate_sha1(char* content, char* output_hash);
void GetContent(char* file_path, char** getContentPtr);
int GetFileSize(char* file_path);
int isInDir(char* file_name, char* dir_path);
int isInTrackedFiles(char* file_name);
int inArray(int argc, char** str_array, char* str);
int inConfig(char* config_file_path, char* keyword_param);
void PrintConfigWarning();
void PrintTrackedFiles();
int isNull(void* ptr, char* message);
void FindMergeBase(char* current_branch_path, char* target_branch_path, char* merge_base_hash);
int isOnBranch(char* check_branch_path, char* check_hash);
int isNameInTree(char* tracked_file_name, char* search_tree_path);
int isTrackInTree(char* tracked_file_line, char* search_tree_path);
void GenerateFilePath(char* path, char* hash);
void GenerateDirPath(char* path, char* hash);
struct dirent* GetEntryFromName(char* entry_name);
void GetTrackingFile(char* dir_path, FILE* tracking_fptr);
void JustGrep(char* keyword);
void NumGrep(char* keyword);
void InsensitiveGrep(char* keyword);
void CatFileType(char* short_hash);
void CatFilePretty(char* short_hash);
int isInsensitive(char c1, char c2);
void strnprint(char* str, int n);
int CheckKalInit();
int CheckForSave();
void FindCommonParentHash(char* branch1_hash, char* branch2_hash, char* common_parent_hash);
int GetMergeWay(char* search_track_line, char* current_branch_path, char* target_branch_path);
void GetTreeLineFromName(char* tree_path, char* new_track_line, char* tracked_file_name);

int main(int argc, char** argv)
{

	if(argc == 1 ||
		strcmp(argv[1], "help") == 0 ||
		strcmp(argv[1], "--help") == 0 || 
		strcmp(argv[1], "-h") == 0) PrintHelp();

	else if(strcmp(argv[1], "--version") == 0 || 
			strcmp(argv[1], "-v") == 0) PrintVersion();

	else if(strcmp(argv[1], "init") == 0 && 
			argc == 2) Init();

	else if(strcmp(argv[1], "status") == 0 && 
			CheckKalInit() &&
			argc == 2) Status();

	else if(strcmp(argv[1], "track") == 0 &&
			CheckKalInit() &&
			argc >= 3) Track(argc, argv);

	else if(strcmp(argv[1], "save") == 0 && 
			strcmp(argv[2], "-m") == 0 &&
			CheckKalInit() &&
			argc == 4) Save(argv[3]);

	else if(strcmp(argv[1], "untrack") == 0 && 
			CheckKalInit() &&
			argc >= 3) UnTrack(argc, argv);

	else if(strcmp(argv[1], "config") == 0 && 
			strcmp(argv[2], "--global") == 0 && 
			strcmp(argv[3], "init.defaultBranch") == 0 && 
			argc == 5) SetDefaultBranchName(argv[4]);

	else if(strcmp(argv[1], "config") == 0 && 
			strcmp(argv[3], "init.defaultBranch") != 0 && 
			CheckKalInit()) SetConfig(argc, argv);

	else if(strcmp(argv[1], "branch") == 0 && 
			CheckKalInit() &&
			argc == 2) ListBranches();

	else if(strcmp(argv[1], "checkout") == 0 && 
			CheckKalInit() &&
			argc == 3) Checkout(argv[2]);

	else if(!strcmp(argv[1], "branch") && 
			CheckKalInit() &&
			!strcmp(argv[2], "-d")) DeleteBranch(argc, argv);

	else if(strcmp(argv[1], "branch") == 0 && 
			CheckKalInit() &&
			argc == 3) GenerateBranch(argc, argv);

	else if(strcmp(argv[1], "merge") == 0 && 
			CheckKalInit() &&
			argc == 5) Merge(argv[2], argv[4]);

	else if(!strcmp(argv[1], "ls-files") && 
			!strcmp(argv[2], "--track") && 
			CheckKalInit() &&
			argc == 3) PrintTrackedFiles();

	else if(!strcmp(argv[1], "save") && 
			!strcmp(argv[2], "--amend") && 
			CheckKalInit() &&
			argc == 3) AmendSave(argv[4]);

	else if(strcmp(argv[1], "reflog") == 0 && 
			CheckKalInit() &&
			argc == 2) Reflog();

	else if(strcmp(argv[1], "log") == 0 && 
			CheckKalInit() &&
			argc == 2) Log();

	else if(strcmp(argv[1], "log") == 0 && 
			strcmp(argv[2], "--oneline") == 0 && 
			CheckKalInit() &&
			argc == 3) Log_OneLine();

	else if(strcmp(argv[1], "clean") == 0 && 
			strcmp(argv[2], "-n") && 
			CheckKalInit() &&
			argc == 3) ListClean();

	else if(strcmp(argv[1], "clean") == 0 && 
			strcmp(argv[2], "-f") && 
			CheckKalInit() &&
			argc == 3) RemoveClean();
		
	else if(strcmp(argv[1], "grep") == 0 && 
			CheckKalInit() &&
			argc == 3) JustGrep(argv[2]);

	else if(strcmp(argv[1], "grep") == 0 && 
			strcmp(argv[2], "-n") == 0 &&
			CheckKalInit() &&
			argc == 4) NumGrep(argv[3]);

	else if(strcmp(argv[1], "grep") == 0 && 
			strcmp(argv[2], "-i") == 0 &&
			CheckKalInit() &&
			argc == 4) InsensitiveGrep(argv[3]);

	else if(strcmp(argv[1], "cat-file") == 0 && 
			strcmp(argv[2], "-t") == 0 &&
			CheckKalInit() &&
			argc == 4) CatFileType(argv[3]);

	else if(strcmp(argv[1], "cat-file") == 0 && 
			strcmp(argv[2], "-p") == 0 &&
			CheckKalInit() &&
			argc == 4) CatFilePretty(argv[3]);

	else printf("'%s' command not found.\n", argv[1]);

	AddLog(argc, argv);
	return 0;
}

/* Fonksiyon İçerikleri */
void PrintHelp()
{
	printf("usage: kal [-v | --version] [-h | --help]\n%11s<command> [<args>]\n\n", " ");
	printf("These are common Kal commands used in various situations:\n\n");
	printf("start a working area\n");
	printf("   %-10sCreate an empty Kal repository or reinitialize an existing one\n\n", "init");
	printf("work on the current change\n");
	printf("   %-10sAdd file contents to the index\n", "track");
	printf("   %-10sRemove files from the working tree and from the index\n\n", "untrack");
	printf("examine the history and state\n");
	printf("   %-10sShow commit logs\n", "log");
	printf("   %-10sShow the working tree status\n\n", "status");
	printf("grow, mark and tweak your common history\n");
	printf("   %-10sList, create, or delete branches\n", "branch");
	printf("   %-10sRecord changes to the repository\n", "save");
	printf("   %-10sJoin two or more development histories together\n", "merge");
}

void PrintVersion()
{
	printf("kal version Pre-Alpha 0.0.1\n");
}

void PrintHint()
{
	printf(YEL "hint: Using 'main' as the name for the initial branch. This default branch name\n" RESET);
	printf(YEL "hint: is subject to change. To configure the initial branch name to use in all\n" RESET);
	printf(YEL "hint: of your new repositories, which will suppress this warning, call:\n" RESET);
	printf(YEL "hint:\n" RESET);
	printf(YEL "hint:  kal config --global init.defaultBranch <name>\n" RESET);
	printf(YEL "hint:\n" RESET);
	printf(YEL "hint: Names commonly chosen instead of 'main' are 'master', 'trunk' and\n" RESET);
	printf(YEL "hint: 'development'. The just-created branch can be renamed via this command:\n" RESET);
	printf(YEL "hint:\n" RESET);
	printf(YEL "hint:  kal branch <name>\n" RESET);
}

void Init()
{
	FILE* create_fptr;
	FILE* config_fptr;
	char cwd[PATH_MAX];
	char default_branch_name[128];
	char default_branch_path[128];
	char config_file_path[128];
	struct passwd* pw;
	char line[128];

	/* çalışma dizinini elde etme */
	if(getcwd(cwd, sizeof(cwd)) == NULL)
	{
		printf("Current working directory could not be found.\n");
		return;
	}

	/* kullanıcıya ait bilgileri (özellikle ev dizininin yolunu) alma */
	if((pw = getpwuid(getuid())) == NULL)
	{
		printf("getpwuid: no password entry\n");
	}

	/* eğer daha önceden "kal" başlatılmamışsa "kal" ı başlat */
	if(stat("./.kal", &st) == -1)
	{
		mkdir("./.kal", 0700);
		mkdir("./.kal/objects", 0700);
		mkdir("./.kal/branches", 0700);
		/* takip edilecek dosyaların takip edileceği dosyayı oluşturma */
		create_fptr = fopen("./.kal/tracked_files.txt", "w");
		fclose(create_fptr);
		/* küresel ayar dosyasında varsayılan dal adı varsa onu kullanma, değilse 'main' dal adını kullanma */
		/* küresel ayar dosyasının yolunu oluşturma */
		sprintf(config_file_path, "%s/.kalconfig", pw->pw_dir);

		config_fptr = fopen(config_file_path, "r");
		if(config_fptr == NULL)
		{
			PrintHint();
			/* hangi dalın mevcut dal olduğunu veya hangi kaydın mevcut kayıt olduğunu takip için HEAD dosyasını oluşturma */
			create_fptr = fopen("./.kal/HEAD", "w");
			fprintf(create_fptr, "branch: ./.kal/branches/main");
			fclose(create_fptr);
			/* başlangıç dalının dosyasını oluşturma */
			create_fptr = fopen("./.kal/branches/main", "w");
			fclose(create_fptr);
		}
		else
		{
			/* ayar dosyasında varsayılan dal isminin atanmamış olma ihtimaline karşı */
			strcpy(default_branch_name, "main");

			/* varsayılan dal adını ayar dosyasından alma */
			while(fgets(line, 128, config_fptr) != NULL)
			{
				if(strcmp(line, "[init]\n") == 0)
				{
					fgets(line, 128, config_fptr);
					sscanf(line, "\tdefaultBranch = %s\n", default_branch_name);
					break;
				}
			}
			fclose(config_fptr);

			if(strcmp(default_branch_name, "main") == 0)
				PrintHint();

			/* hangi dalın mevcut dal olduğunu veya hangi kaydın mevcut kayıt olduğunu takip için HEAD dosyasını oluşturma */
			create_fptr = fopen("./.kal/HEAD", "w");
			fprintf(create_fptr, "branch: ./.kal/branches/%s", default_branch_name);
			fclose(create_fptr);

			/* başlangıç dalının yolunu oluşturma */
			sprintf(default_branch_path, "./.kal/branches/%s", default_branch_name);
			create_fptr = fopen(default_branch_path, "w");
			fclose(create_fptr);
		}

		printf("Initialized empty kal repository in %s/.kal\n", cwd);
		return;
	}
	else
	{
		printf("kal was already initialized.\n");
		return;
	}
}

void Status()
{
	/* kal eğer başlatıldıysa devam etsin, başlatılmadıysa kullanıcıyı bilgilendirme */
	if(!isInDir(".kal", "."))
	{
		printf("fatal: not a kal repository (or any of the parent directories): .kal\n");
		return;
	}

	/* var olan her dosyanın hash ini oluştur ve tracked_files.txt dosyasındaki 
	isim ve hash lerle karşılaştır */
	FILE* tracked_files_txt_ptr;
	FILE* tracked_file_ptr;
	FILE* ignore_fptr;
	FILE* fptr;
	int ignore_exists;
	int tracked_files_txt_size;
	DIR* dir;
	struct dirent* entry;
	blob_t new_blob;
	tree_t new_tree;
	char* content;
	char line[128];
	char file_name[32];
	char file_hash[41];
	char item_name[32];
	char item_hash[41];
	int exists;
	int remove;
	char command[128];
	char entry_path[PATH_MAX];
	char tree_file_path[256];
	char entry_name[64];

	/* klasörün içeriğini okuyabilmek için klasörü açma */
	dir = opendir(".");
	if(dir == NULL)
	{
		printf("directory could not be opened to get files!\n");
		return;
	}

	/* .kalignore dosyasının olup olmadığını kontrol etme */
	ignore_exists = 0;
	ignore_fptr = fopen("./.kalignore", "r");
	if(ignore_fptr == NULL) ignore_exists = 0;
	else
	{
		ignore_exists = 1;
		fclose(ignore_fptr);
	}

	/* tracked_files.txt dosyasının boyutunu alma */
	tracked_files_txt_size = GetFileSize("./.kal/tracked_files.txt");
 
	/* tracked_files.txt boşsa tüm ögeleri takip edilmeyen olarak gösterme */
	if(tracked_files_txt_size == 0)
	{
		printf("Untracked files:\n");
		printf("  (use \"kal track <file>...\" to include in what will be saved)\n");
		while((entry = readdir(dir)) != NULL)
		{
			if((entry->d_name)[0] != '.') 
			{
				/* eğer .kalignore dosyası mevcutsa ve dosya ismi bu dosyada değilse yazdır */
				if(ignore_exists)
					if(!isInIgnoreFile(entry->d_name))
					{
						printf(RED "\t%s\n" RESET, entry->d_name);
						continue;
					}
				printf(RED "\t%s\n" RESET, entry->d_name);
			}
		}
		printf("nothing added to save but untracked files present (use \"kal track\" to track)\n");
		closedir(dir);
		return;
	}
	/* eğer tracked_files.txt boş değilse (tüm dosyaların hash lerini oluştur ve eğer isimleri dosyada varsa 
	dosyanın güncellenip güncellenmediğini öğrenmek için hash leri karşılaştır) (klasörler için de yapılmalı)*/
	else
	{
		printf("Tracked files:\n"); /* Güncellenmeli */
		while((entry = readdir(dir)) != NULL)
		{
			/* eğer dosyanın ismi . ile başlıyorsa bir sonraki dosyaya geç */
			if(entry->d_name[0] == '.') continue;

			/* eğer öge bir dosya ise */
			if(entry->d_type == 8)
			{
				GetContent(entry->d_name, &content);
	
				calculate_sha1(content, new_blob.hash);
				tracked_files_txt_ptr = fopen("./.kal/tracked_files.txt", "r");
				if(isNull(tracked_files_txt_ptr, "tracked_files.txt could not be opened! (Status)\n"))
					return;
				while(fgets(line, 128, tracked_files_txt_ptr) != NULL)
				{
					sscanf(line, "%s -> %s", file_name, file_hash);
					/* eğer dosya ismi tracked_files.txt dosyasında varsa ve dosya güncellenmemişse takip edilenler kısmında yazdır */
					if(strcmp(file_name, entry->d_name) == 0 && strcmp(file_hash, new_blob.hash) == 0)
					{
						/* eğer .kalignore dosyası mevcutsa ve dosya ismi bu dosyada değilse yazdır cc*/
						if(ignore_exists)
						{
							if(!isInIgnoreFile(entry->d_name))
							{
								printf(GRN "\t%s\n" RESET, entry->d_name);
								break;
							}
						}
						printf(GRN "\t%s\n" RESET, entry->d_name);
						break;
					}
				}
				fclose(tracked_files_txt_ptr);
			}
			/* eğer öge bir klasör ise önce klasörün tree hash ini elde etme ve sonra da tracked_files.txt de arama yapma */
			else if(entry->d_type == 4)
			{
				/* klasör yolunun bulunup bulunamadığı kontrol edilmeli 
				(iç içe olan şeyleri doğru bulamadığı için farklı bir yöntem kullanıldı) */
				realpath(entry->d_name, entry_path);
				sprintf(entry_path, "./%s", entry->d_name);

				sprintf(tree_file_path, "./.kal/%s_tracked_files.txt", entry->d_name);

				strcpy(entry_name, entry->d_name);

				fptr = fopen(tree_file_path, "w");
				if(isNull(fptr, "temp tree file could not be opened! (Status)\n"))
					return;
				GetTrackingFile(entry_path, fptr);
				fclose(fptr);

				GetContent(tree_file_path, &(new_tree.content));
				calculate_sha1(new_tree.content, new_tree.hash);

				tracked_files_txt_ptr = fopen("./.kal/tracked_files.txt", "r");
				if(isNull(tracked_files_txt_ptr, "tracked_files.txt could not be opened! (Status)\n"))
					return;
				while(fgets(line, 128, tracked_files_txt_ptr) != NULL)
				{
					sscanf(line, "%s -> %s", item_name, item_hash);
					/* eğer dosya ismi tracked_files.txt dosyasında varsa ve dosya güncellenmemişse takip edilenler kısmında yazdır */
					if(strcmp(item_name, entry_name) == 0 && strcmp(item_hash, new_tree.hash) == 0)
					{
						/* eğer .kalignore dosyası mevcutsa ve dosya ismi bu dosyada değilse yazdır cc*/
						if(ignore_exists)
						{
							if(!isInIgnoreFile(entry_name))
							{
								printf(GRN "\t%s\n" RESET, entry_name);
								break;
							}
						}
						printf(GRN "\t%s\n" RESET, entry_name);
						break;
					}
				}
				fclose(tracked_files_txt_ptr);

				sprintf(command, "rm -f %s", tree_file_path);
				system(command);
				free(new_tree.content);
			}
			
		}
		closedir(dir);

		/* klasördeki tüm elemanları alarak hash lerini oluşturma ve daha sonra da
		 tracked_files.txt dosyasını dolaşarak varlıklarını kontrol etme */
		dir = opendir(".");
		printf("Untracked files:\n");
		while((entry = readdir(dir)) != NULL)
		{
			exists = 0;
			remove = 0;

			/* eğer dosyanın ismi . ile başlıyorsa dosyanın takip edilip edilmediğini kontrol etmeden
			bir sonraki dosyaya geç */
			if(entry->d_name[0] == '.') continue;
	
			/* eğer öge türü dosya ise */
			if(entry->d_type == 8)
			{
				GetContent(entry->d_name, &content);
				calculate_sha1(content, new_blob.hash);

				tracked_files_txt_ptr = fopen("./.kal/tracked_files.txt", "r");
				if(isNull(tracked_files_txt_ptr, "tracked_files.txt could not be opened! (Status)\n"))
					return;

				fptr = fopen("./.kal/check_tracked_files.txt", "w");
				if(isNull(fptr, "check_tracked_files.txt could not be opened! (Status)\n"))
					return;

				while(fgets(line, 128, tracked_files_txt_ptr) != NULL)
				{
					sscanf(line, "%s -> %s", file_name, file_hash);
					if(strcmp(file_name, entry->d_name) == 0 && strcmp(file_hash, new_blob.hash) == 0)
					{
						exists = 1;
						break;
					}
					/* eğer dosya güncellenmişse tracked_files.txt dosyasından dosya ismini ve hash ini kaldır ve
					objects klasöründen de gerekli kaldırmaları yap */
					else if(strcmp(file_name, entry->d_name) == 0 && strcmp(file_hash, new_blob.hash) != 0)
					{
						remove = 1;
						/* objects klasöründen güncellenen (eskiden takip edilen) dosyaya ait klasör ve dosyayı kaldırma */
						GenerateDirPath(new_blob.dir_path, new_blob.hash);
						sprintf(command, "rm -rf ./.kal/objects/%s", new_blob.dir_path);
						system(command);
						continue; /* dikkat! */
					}
					
					fprintf(fptr, "%s", line);
				}

				if(ignore_exists && !exists)
				{
					if(!isInIgnoreFile(entry->d_name))
					{
						printf(RED "\t%s\n" RESET, entry->d_name);
					}
				}
				else if(!exists) printf(RED "\t%s\n" RESET, entry->d_name);

				fclose(fptr);
				fclose(tracked_files_txt_ptr);

				/* daha önceden takip edilirken sonradan güncellenen bir dosya varsa o dosyayı tracked_files.txt den çıkarma */
				if(remove)
				{
					fptr = fopen("./.kal/check_tracked_files.txt", "r");
					if(isNull(fptr, "check_tracked_files.txt could not be opened! (Status)\n"))
						return;

					tracked_files_txt_ptr = fopen("./.kal/tracked_files.txt", "w");
					if(isNull(tracked_files_txt_ptr, "./.kal/tracked_files.txt could not be opened! (Status)\n"))
						return;

					while(fgets(line, 128, fptr) != NULL)
					{
						fprintf(tracked_files_txt_ptr, "%s", line);
					}

					fclose(fptr);
					fclose(tracked_files_txt_ptr);
				}
			}
			/* eğer öge türü klasör ise */
			else if(entry->d_type == 4)
			{
				/* klasör yolunun bulunup bulunamadığı kontrol edilmeli */
				realpath(entry->d_name, entry_path);

				sprintf(tree_file_path, "./.kal/%s_tracked_files.txt", entry->d_name);

				strcpy(entry_name, entry->d_name);

				fptr = fopen(tree_file_path, "w");
				if(isNull(fptr, "temp tree file could not be opened! (Status)\n"))
					return;
				GetTrackingFile(entry_path, fptr);
				fclose(fptr);

				GetContent(tree_file_path, &(new_tree.content));
				calculate_sha1(new_tree.content, new_tree.hash);

				tracked_files_txt_ptr = fopen("./.kal/tracked_files.txt", "r");
				if(isNull(tracked_files_txt_ptr, "tracked_files.txt could not be opened! (Status)\n"))
					return;

				fptr = fopen("./.kal/check_tracked_files.txt", "w");
				if(isNull(fptr, "check_tracked_files.txt could not be opened! (Status)\n"))
					return;

				while(fgets(line, 128, tracked_files_txt_ptr) != NULL)
				{
					sscanf(line, "%s -> %s", item_name, item_hash);
					if(strcmp(item_name, entry_name) == 0 && strcmp(item_hash, new_tree.hash) == 0)
					{
						exists = 1;
						break;
					}
					/* eğer dosya güncellenmişse tracked_files.txt dosyasından dosya ismini ve hash ini kaldır ve
					objects klasöründen de gerekli kaldırmaları yap */
					else if(strcmp(item_name, entry_name) == 0 && strcmp(item_hash, new_tree.hash) != 0)
					{
						remove = 1;
						/* objects klasöründen güncellenen (eskiden takip edilen) dosyaya ait klasör ve dosyayı kaldırma */
						GenerateDirPath(new_tree.dir_path, new_tree.hash);
						sprintf(command, "rm -rf ./.kal/objects/%s", new_tree.dir_path);
						system(command);
						continue; /* dikkat! */
					}
					
					fprintf(fptr, "%s", line);
				}

				if(ignore_exists && !exists)
				{
					if(!isInIgnoreFile(entry_name))
					{
						printf(RED "\t%s\n" RESET, entry_name);
					}
				}
				else if(!exists) printf(RED "\t%s\n" RESET, entry_name);

				fclose(fptr);
				fclose(tracked_files_txt_ptr);

				/* daha önceden takip edilirken sonradan güncellenen bir dosya varsa o dosyayı tracked_files.txt den çıkarma */
				if(remove)
				{
					fptr = fopen("./.kal/check_tracked_files.txt", "r");
					if(isNull(fptr, "check_tracked_files.txt could not be opened! (Status)\n"))
						return;

					tracked_files_txt_ptr = fopen("./.kal/tracked_files.txt", "w");
					if(isNull(tracked_files_txt_ptr, "./.kal/tracked_files.txt could not be opened! (Status)\n"))
						return;

					while(fgets(line, 128, fptr) != NULL)
					{
						fprintf(tracked_files_txt_ptr, "%s", line);
					}

					fclose(fptr);
					fclose(tracked_files_txt_ptr);
				}

				sprintf(command, "rm -f %s", tree_file_path);
				system(command);
				free(new_tree.content);
			}
			
			sprintf(command, "rm -f check_tracked_files.txt");
			system(command);
		}
		closedir(dir);
		return;
	}
}

void Track(int argc, char** argv)
{
	FILE* create_file_ptr;
	FILE* tracked_files_txt_ptr;
	FILE* fptr;
	char command[1024];
	int i;
	char* content;
	char c;
	blob_t new_blob;
	tree_t new_tree;
	char dir_path[18];
	char file_path[54];
	char line[128];
	char file_name[32];
	char file_hash[41];
	int track_exists = 0;
	int track_update = 0;
	char entry_path[PATH_MAX];
	char entry_name[64]; /* entry->d_name e sonradan nedenini bilmediğim bir şeyden dolayı ulaşılamıyor */

	DIR* dir;
	struct dirent* entry;
	int longest_name_length = 0;
	int file_count = 0;
	char** file_args;

	char tree_file_path[512];
	int paramc;
	char** paramv;

	/* parametre olarak '.' verilmesi durumunda parametrelerin tüm dosyalar olarak alınması */
	if(argc == 3 && (!strcmp(argv[2], ".") || !strcmp(argv[2], "-a") || !strcmp(argv[2], "--all") || !strcmp(argv[2], "-A")))
	{
		dir = opendir(".");
		if(dir == NULL)
		{
			printf("directory could not be opened! (Track 1)\n");
			return;
		}

		/* dosyadaki dosya sayısını ve en uzun dosya ismini bulma */
		while((entry = readdir(dir)) != NULL)
		{
			if((entry->d_name)[0] != '.' && !isInTrackedFiles(entry->d_name)) file_count++;
			if((entry->d_name)[0] != '.' && !isInTrackedFiles(entry->d_name) && strlen(entry->d_name) > longest_name_length) 
				longest_name_length = strlen(entry->d_name);
		}
		closedir(dir);

		file_args = (char**)malloc(sizeof(char*) * file_count);
		for(i = 0; i < file_count; i++)
			file_args[i] = (char*)malloc(sizeof(char) * longest_name_length);

		
		/* klasördeki dosyaların isimlerini argv ye aktarma */
		dir = opendir(".");
		if(dir == NULL)
		{
			printf("directory could not be opened! (Track 2)\n");
			return;
		}

		i = 0;
		while((entry = readdir(dir)) != NULL)
		{
			if((entry->d_name)[0] != '.' && !isInTrackedFiles(entry->d_name)) strcpy(file_args[i++], entry->d_name);
		}
		closedir(dir);
	}
	else
	{
		file_args = &(argv[2]);
		file_count = argc - 2;
	}
	
	for(i = 0; i < file_count; i++)
	{
		/* takip edilmesi istenen dosyanın klasörde bulunmaması durumunda kullanıcının bilgilendirilmesi */
		if( !isInDir(file_args[i], ".") )
		{
			printf("file '%s' does not exist!\n", file_args[i]);
			continue;
		}

		/* parametre adına sahip olan ögenin dosya mı klasör mü olup olmadığını öğrenme */
		entry = GetEntryFromName(file_args[i]);
		/* öge eğer klasörse */
		if(entry->d_type == 4)
		{
			/* klasör yolunun bulunup bulunamadığı kontrol edilmeli */
			realpath(entry->d_name, entry_path);

			sprintf(tree_file_path, "./.kal/%s_tracked_files.txt", entry->d_name);

			strcpy(entry_name, entry->d_name);

			fptr = fopen(tree_file_path, "w");
			if(isNull(fptr, "temp tree file could not be opened! (Track)\n"))
				return;
			GetTrackingFile(entry_path, fptr);
			fclose(fptr);

			GetContent(tree_file_path, &(new_tree.content));
			calculate_sha1(new_tree.content, new_tree.hash);

			/* klasörün tree dosyasının içeriğini depolamak için tree dosyasının hash i ile klasör ve dosya oluşturma 
			ve tree dosyasının içeriğini sıkıştırarak ana dosyaya aktarma */
			GenerateDirPath(new_tree.dir_path, new_tree.hash);
			GenerateFilePath(new_tree.file_path, new_tree.hash);

			/* hash in ilk 2 harfini kullanarak klasör oluşturulmalı */
			if(stat(new_tree.dir_path, &st) == -1)
			{
				mkdir(new_tree.dir_path, 0700);
			}

			/* o klasörün içine hash in geri kalanını kullanan dosya oluşturulmalı */
			fptr = fopen(new_tree.file_path, "w");
			fclose(fptr);

			/* o dosyanın içine içerik sıkıştırılarak yazılmalı */
			sprintf(command, "gzip -c %s | base64 > %s", tree_file_path, new_tree.file_path);
			system(command);

			/* eğer tracked_files.txt dosyası boşsa ilk takip edilecek dosyanın adını yazsın */
			if(GetFileSize("./.kal/tracked_files.txt") == 0)
			{
				tracked_files_txt_ptr = fopen("./.kal/tracked_files.txt", "a+");
				fprintf(tracked_files_txt_ptr, "%s -> %s\n", entry_name, new_tree.hash);
				fclose(tracked_files_txt_ptr);
				printf("%s is now being tracked.\n", entry_name);
			}
			/* tracked_files.txt ye takip edilen dosyanın adı ve hash i kaydedilmeli
				eğer var olan bir dosya ise ya aynı kaydı kullansın
				eğer değiştirilmiş bir dosya ise sadece hash kaydını değiştirsin */
			else
			{
				track_exists = 0;
				track_update = 0;
				tracked_files_txt_ptr = fopen("./.kal/tracked_files.txt", "r+");
				while(fgets(line, 128, tracked_files_txt_ptr) != NULL)
				{
					sscanf(line, "%s -> %s", file_name, file_hash);
					if(strcmp(file_name, entry->d_name) == 0 && strcmp(file_hash, new_tree.hash) != 0)
					{
						/* hash i güncelle */
						track_update = 1;
						continue;
					}
					else if(strcmp(file_name, file_args[i]) == 0 && strcmp(file_hash, new_blob.hash) == 0)
					{
						printf("%s has already beeing tracked!\n", file_args[i]);
						track_exists = 1;
						break;
					}
				}
				fclose(tracked_files_txt_ptr);
				/* eğer dosyanın adı tracked_files.txt dosyasında dosyanın en sonuna yaz ve takibe başla */
				if( !track_exists )
				{
					tracked_files_txt_ptr = fopen("./.kal/tracked_files.txt", "a+");
					fprintf(tracked_files_txt_ptr, "%s -> %s\n", entry_name, new_tree.hash);
					printf("%s is now being tracked.\n", entry_name);
					fclose(tracked_files_txt_ptr);
				}
				/* eğer var olan bir kayıt güncellenecekse */
				if( track_update )
				{
					fptr = fopen("./.kal/temp_tracked_files.txt", "r");
					if(isNull(fptr, "temp_tracked_files.txt could not be opened! (Track)\n"))
						return;

					tracked_files_txt_ptr = fopen("./.kal/tracked_files.txt", "w");
					if(isNull(tracked_files_txt_ptr, "tracked_files.txt could not be opened! (Track)\n"))
						return;

					while(fgets(line, 128, fptr) != NULL)
					{
						fprintf(tracked_files_txt_ptr, "%s", line);
					}

					fclose(tracked_files_txt_ptr);
					fclose(fptr);
				}
			}
			
			sprintf(command, "rm -f ./.kal/temp_tracked_files.txt");
			system(command);
			free(new_tree.content);
		}
		/* eğer öge bir klasör değil bir dosya ise */
		else if(entry->d_type == 8)
		{
			/* takip edilecek dosyanın içeriğinin alınması */
			GetContent(file_args[i], &content); 

			/* içeriği kullanarak şifreleme yapmalı */
			calculate_sha1(content, new_blob.hash);
			new_blob.size = strlen(content);
			new_blob.content = (char*)malloc(sizeof(char) * strlen(content));
			strcpy(new_blob.content, content);

			/* oluşturulacak klasör ve dosyaların yollarının hazırlanması */
			GenerateDirPath(new_blob.dir_path, new_blob.hash);
			GenerateFilePath(new_blob.file_path, new_blob.hash);

			/* hash in ilk 2 harfini kullanarak klasör oluşturulmalı */
			if(stat(new_blob.dir_path, &st) == -1)
			{
				mkdir(new_blob.dir_path, 0700);
			}

			/* o klasörün içine hash in geri kalanını kullanan dosya oluşturulmalı */
			create_file_ptr = fopen(new_blob.file_path, "w");
			fclose(create_file_ptr);

			/* o dosyanın içine içerik sıkıştırılarak yazılmalı */
			sprintf(command, "gzip -c %s > compresed.gz", file_args[i]);
			system(command);
			sprintf(command, "base64 compresed.gz > %s", new_blob.file_path);
			system(command);
			sprintf(command, "rm -f compresed.gz");
			system(command);
			/* eğer tracked_files.txt dosyası boşsa ilk takip edilecek dosyanın adını yazsın */
			if(GetFileSize("./.kal/tracked_files.txt") == 0)
			{
				tracked_files_txt_ptr = fopen("./.kal/tracked_files.txt", "a+");
				fprintf(tracked_files_txt_ptr, "%s -> %s\n", file_args[i], new_blob.hash);
				printf("%s is now being tracked.\n", file_args[i]);
				fclose(tracked_files_txt_ptr);
			}
			/* tracked_files.txt ye takip edilen dosyanın adı ve hash i kaydedilmeli
				eğer var olan bir dosya ise ya aynı kaydı kullansın
				eğer değiştirilmiş bir dosya ise sadece hash kaydını değiştirsin */
			else
			{
				track_exists = 0;
				track_update = 0;
				tracked_files_txt_ptr = fopen("./.kal/tracked_files.txt", "r+");
				if(isNull(tracked_files_txt_ptr, "tracked_filest.txt could not be opened! (Track)\n"))
					return;
				fptr = fopen("./.kal/temp_tracked_files.txt", "w");
				if(isNull(fptr, "temp_tracked_files.txt could not be opened! (Track)\n"))
					return;
				while(fgets(line, 128, tracked_files_txt_ptr) != NULL)
				{
					sscanf(line, "%s -> %s", file_name, file_hash);
					if(strcmp(file_name, file_args[i]) == 0 && strcmp(file_hash, new_blob.hash) != 0)
					{
						/* hash i güncelle */
						fprintf(fptr, "%s -> %s", file_name, new_blob.hash);
						
						fclose(fptr);
						track_update = 1;
						continue;
						
					}
					else if(strcmp(file_name, file_args[i]) == 0 && strcmp(file_hash, new_blob.hash) == 0)
					{
						printf("%s has already beeing tracked!\n", file_args[i]);
						track_exists = 1;
						break;
					}
					fprintf(fptr, "%s", line);
				}
				fclose(fptr);
				fclose(tracked_files_txt_ptr);
				/* eğer dosyanın adı tracked_files.txt dosyasında dosyanın en sonuna yaz ve takibe başla */
				if( !track_exists )
				{
					tracked_files_txt_ptr = fopen("./.kal/tracked_files.txt", "a+");
					fprintf(tracked_files_txt_ptr, "%s -> %s\n", file_args[i], new_blob.hash);
					printf("%s is now being tracked.\n", file_args[i]);
					fclose(tracked_files_txt_ptr);
				}
				/* eğer var olan bir kayıt güncellenecekse */
				if( track_update )
				{
					fptr = fopen("./.kal/temp_tracked_files.txt", "r");
					if(isNull(fptr, "temp_tracked_files.txt could not be opened! (Track)\n"))
						return;

					tracked_files_txt_ptr = fopen("./.kal/tracked_files.txt", "w");
					if(isNull(tracked_files_txt_ptr, "tracked_files.txt could not be opened! (Track)\n"))
						return;

					while(fgets(line, 128, fptr) != NULL)
					{
						fprintf(tracked_files_txt_ptr, "%s", line);
					}

					sprintf(command, "rm -f ./.kal/temp_tracked_files.txt");
					system(command);

					fclose(tracked_files_txt_ptr);
					fclose(fptr);
			}
			}

			free(new_blob.content);
			free(content);
		}
		/* eğer içerik dosya da klasör de değilse kullanıcıyı bilgilendirme (değiştirilebilir) */
		else
		{
			printf("entry %s is neither a regular file nor a directory!\n", entry->d_name);
			continue;
		}
	}

	/* eğer parametre olarak '.' verildiyse malloc ile tahsis edilen belleği serbest bırakma */
	if(argc == 3 && (!strcmp(argv[2], ".") || !strcmp(argv[2], "-a") || !strcmp(argv[2], "--all") || !strcmp(argv[2], "-A")))
	{
		for(i = 0; i < file_count; i++)
			free(file_args[i]);
		free(file_args);
	}

    return;
}

void UnTrack(int argc, char** argv)
{
	/* kal eğer başlatıldıysa devam etsin, başlatılmadıysa kullanıcıyı bilgilendirme */
	if(!isInDir(".kal", "."))
	{
		printf("fatal: not a kal repository (or any of the parent directories): .kal\n");
		return;
	}

	FILE* fptr;
	FILE* fptr_temp;
	char line[128];
	char name[32];
	char hash[48];
	int i;
	char c;
	int index = 0;
	char command[128];
	char dir_path[18];
	char file_path[54];
	char dir_name[3];
	char file_name[39];
	DIR* rm_dir;
	struct dirent* rm_entry;

	DIR* dir;
	struct dirent* entry;
	int longest_name_length = 0;
	int file_count = 0;
	char** file_args;

	/* parametre olarak '.' verilmesi durumunda parametrelerin tüm dosyalar olarak alınması */
	if(argc == 3 && strcmp(argv[2], ".") == 0)
	{
		dir = opendir(".");
		if(dir == NULL)
		{
			printf("directory could not be opened! (UnTrack 1)\n");
			return;
		}

		/* dosyadaki dosya sayısını ve en uzun dosya ismini bulma */
		while((entry = readdir(dir)) != NULL)
		{
			if((entry->d_name)[0] != '.') file_count++;
			if((entry->d_name)[0] != '.' && strlen(entry->d_name) > longest_name_length) longest_name_length = strlen(entry->d_name);
		}
		closedir(dir);

		/* klasördeki dosyaların isimlerinin aktarılacağı diziyi oluşturma */
		file_args = (char**)malloc(sizeof(char*) * file_count);
		for(i = 0; i < file_count; i++)
			file_args[i] = (char*)malloc(sizeof(char) * longest_name_length);

		
		/* klasördeki dosyaların isimlerini argv ye aktarma */
		dir = opendir(".");
		if(dir == NULL)
		{
			printf("directory could not be opened! (UnTrack 2)\n");
			return;
		}

		i = 0;
		while((entry = readdir(dir)) != NULL)
		{
			if((entry->d_name)[0] != '.') strcpy(file_args[i++], entry->d_name);
		}
		closedir(dir);
	}
	else
	{
		file_args = &(argv[2]);
		file_count = argc - 2;
	}

	/* parametre olarak verilen dosya veya dosyalardan zaten takip edilmeyenleri yazdırma */
	for(i = 0; i < file_count; i++)
	{
		if(!isInTrackedFiles(file_args[i]))
		{
			printf("%s is already not being tracked!\n", file_args[i]);
		}
	}

	/* güncellenmiş takip edilecek dosyaların listesini oluşturma */
	fptr_temp = fopen("./.kal/temp.txt", "w");
	fptr = fopen("./.kal/tracked_files.txt", "r+");
	while(fgets(line, 128, fptr) != NULL)
	{
		/* her satırdaki isim ve hash leri alma */
		sscanf(line, "%s -> %s", name, hash);
		/* satırdaki ismin istenilen isim olup olmadığını kontrol etme 
		eğer takibi bırakılmak istenen dosyanın adı ile aynı ise o satırı sil */
		if(inArray(file_count, file_args, name)) 
		{
			/* silinecek klasör ve dosyanın isimlerinin alınması */
			strncpy(dir_name, hash, 2);
			dir_name[2] = '\0';
			strcpy(file_name, &(hash[2]));
			
			/* silinecek klasör ve dosyaların yollarının hazırlanması */
			GenerateDirPath(dir_path, hash);
			GenerateFilePath(file_path, hash);
			
			/* silinecek klasör ve dosyanın objects klasöründe aranması */
			dir = opendir("./.kal/objects");
			while((entry = readdir(dir)) != NULL)
			{
				if(strcmp(entry->d_name, dir_name) == 0)
				{
					rm_dir = opendir(dir_path);
					while((rm_entry = readdir(rm_dir)) != NULL)
					{
						if(strncmp(rm_entry->d_name, file_name, 35) == 0) /* rm_entry->d_name in uzunluğu 256 değil 36 falan oldu nedenini araştır */
						{
							sprintf(command, "rm -rf %s", dir_path);
							system(command);
						}
					}
					closedir(rm_dir);
				}
			}
			closedir(dir);
			
			printf("%s is now being not tracked.\n", name);
			continue;
		}
		fprintf(fptr_temp, "%s -> %s\n", name, hash);
	}
	fclose(fptr);
	fclose(fptr_temp);

	/* tracked_files.txt dosyasına güncellenmiş takip edilecek dosyaları yazma */
	fptr = fopen("./.kal/tracked_files.txt", "w");
	fptr_temp = fopen("./.kal/temp.txt", "r");
	while(fgets(line, 128, fptr_temp) != NULL)
	{
		fprintf(fptr, "%s", line);
	}
	fclose(fptr_temp);
	fclose(fptr);

	/* eğer parametre olarak '.' verildiyse malloc ile tahsis edilen belleği serbest bırakma */
	if(argc == 3 && strcmp(argv[2], ".") == 0)
	{
		for(i = 0; i < file_count; i++)
			free(file_args[i]);
		free(file_args);
	}

	sprintf(command, "rm -f ./.kal/temp.txt");
	system(command);
}

void Save(char* save_message)
{
	FILE* fptr;
	FILE* kalconfig_ptr;
	FILE* tree_ptr;
	FILE* save_ptr;
	struct passwd* pw;
	char config_file_path[64];
	char line[128];
	char keyword[16];
	int check = 0;
	int file_size;
	char* content;
	tree_t new_tree;
	save_t new_save;
	char command[32];
	char user_name[64];
	char user_email[64];
	time_t rawtime;
	struct tm* timeinfo;
	char save_time[64];
	char last_save_hash[41];
	char current_branch_path[64];
	char short_hash[8];
	
	/* ev dizinin yolunu alma */
	if((pw = getpwuid(getuid())) == NULL)
	{
		printf("getpwuid: no password entry\n");
		return;
	}

	/* eğer takip edilecek dosya yoksa kullanıcı bilgilendirilmeli */
	file_size = GetFileSize("./.kal/tracked_files.txt");
	if(file_size == 0)
	{
		printf("nothing to commit, tracked_files.txt file is clean\n");
		return;
	}

	/* kullanıcının bilgilerinin atanıp atanmadığını kontrol etme 
	eğer atanmadıysa kullanıcıyı bilgilendirme */
	if(!CheckForSave())
		return;

	/* tree nesnesinde kullanmak için tracked_files.txt dosyasının boyutunu öğrenme */
	fptr = fopen("./.kal/tracked_files.txt", "r");
	fseek(fptr, 0, SEEK_END);
	file_size = ftell(fptr);
	fclose(fptr);

	/* tree nesnesine( dosyasına ) tracked_files.txt deki verileri yazma */
	fptr = fopen("./.kal/tracked_files.txt", "r");
	if(isNull(fptr, "tracked_files.txt could not be opened! (Save)\n")) return;
	tree_ptr = fopen("./.kal/tree.txt", "w");
	if(isNull(fptr, "tree.txt could not be opened! (Save)\n")) return;
	fprintf(tree_ptr, "tree\n");
	while(fgets(line, 128, fptr) != NULL)
	{
		fprintf(tree_ptr, "%s", line);
	}
	fclose(tree_ptr);
	fclose(fptr);

	/* tree.txt dosyasının içeriğini alma, hash oluşturma ve içeriği dosyaya kaydetme */
	GetContent("./.kal/tree.txt", &(new_tree.content));
	calculate_sha1(new_tree.content, new_tree.hash);

	/* oluşturulacak klasör ve dosyaların yollarının hazırlanması */
	GenerateDirPath(new_tree.dir_path, new_tree.hash);
	GenerateFilePath(new_tree.file_path, new_tree.hash);

	/* hash in ilk 2 harfini kullanarak klasör oluşturulması */
	if(stat(new_tree.dir_path, &st) == -1)
	{
		mkdir(new_tree.dir_path, 0700);
	}

	/* o klasörün içine hash in geri kalanını kullanan dosya oluşturulması */
	fptr = fopen(new_tree.file_path, "w");
	fclose(fptr);

	/* tree.txt dosyasının içeriğinin ana(düzeltilebilir) dosyaya aktarılması */
	tree_ptr = fopen("./.kal/tree.txt", "r");
	if(isNull(tree_ptr, "tree.txt could not be opened! (Save)\n")) return;
	fptr = fopen(new_tree.file_path, "w");
	if(isNull(fptr, "tree object file could not be opened! (Save)\n")) return;
	while(fgets(line, 128, tree_ptr) != NULL)
	{
		fprintf(fptr, "%s", line);
	}
	fclose(fptr);
	fclose(tree_ptr);
	free(new_tree.content);
	/* tree.txt dosyasının silinmesi */
	sprintf(command, "rm -f ./.kal/tree.txt");
	system(command);

	/* save dosyasına yazmak için config dosyasındaki kullanıcı adı ve email bilgilerini alma */
	strcpy(config_file_path, "./.kal/config");
	fptr = fopen(config_file_path, "r");
	if(fptr == NULL)
	{
		/* ayar dosyasının yolunun hazırlanması ve dosyanın açılmaya çalışılması */
		strcpy(config_file_path, pw->pw_dir);
		strcat(config_file_path, "/.kalconfig");
		fptr = fopen(config_file_path, "r");
	}
	if(isNull(fptr, "config file could not be opened! (Save)\n")) return;
	while(fgets(line, 128, fptr) != NULL)
	{
		if(strcmp(line, "[user]\n") == 0) continue;
		sscanf(line, "\t%s", keyword);
		if(strcmp(keyword, "name") == 0) 
		{
			strcpy(user_name, &(line[8]));
			user_name[strlen(user_name) - 1] = '\0';
		}
		else if(strcmp(keyword, "email") == 0) sscanf(line, "\temail = %s\n", user_email);
	}
	fclose(fptr);

	/* save dosyasına yazmak için kayıt zamanını alma */
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strcpy(save_time, asctime(timeinfo));

	/* save dosyasının parent kısmına yazmak için mevcut dalın yolunu alıp
	mevcut dalın dosyasından son yapılan kaydın hash ini alma */
	fptr = fopen("./.kal/HEAD", "r");
	if(isNull(fptr, "HEAD could not be opened! (Save)\n"))
		return;
	fscanf(fptr, "branch: %s", current_branch_path);
	fclose(fptr);

	fptr = fopen(current_branch_path, "r");
	if(isNull(fptr, "current branch file could not be opened! (Save)\n")) return;
	if(fgets(line, 128, fptr) == NULL)
	{
		strcpy(last_save_hash, "NONE");
		fclose(fptr);
	}
	else
	{
		sscanf(line, "%s", last_save_hash);
		fclose(fptr);
	}

	/* save dosyasını oluşturma ve içeriği yazma */
	save_ptr = fopen("./.kal/save.txt", "w");
	if(isNull(save_ptr, "save.txt could not be opened! (Save)\n")) return;
	fprintf(save_ptr, "save\n");
	fprintf(save_ptr, "tree %s\n", new_tree.hash);
	fprintf(save_ptr, "parent %s\n", last_save_hash);
	fprintf(save_ptr, "author %s | %s\n", user_name, user_email);
	fprintf(save_ptr, "committer %s | %s\n", user_name, user_email);
	fprintf(save_ptr, "save time %s\n", save_time);
	fprintf(save_ptr, "save message: %s", save_message);
	fclose(save_ptr);

	/* save.txt dosyasının içeriğini alma, hash oluşturma ve içeriği dosyaya kaydetme */
	GetContent("./.kal/save.txt", &(new_save.content));
	calculate_sha1(new_save.content, new_save.hash);

	/* oluşturulacak klasör ve dosyaların yollarının hazırlanması */
	GenerateDirPath(new_save.dir_path, new_save.hash);
	GenerateFilePath(new_save.file_path, new_save.hash);

	/* hash in ilk 2 harfini kullanarak klasör oluşturulması */
	if(stat(new_save.dir_path, &st) == -1)
	{
		mkdir(new_save.dir_path, 0700);
	}

	/* o klasörün içine hash in geri kalanını kullanan dosya oluşturulması */
	fptr = fopen(new_save.file_path, "w");
	fclose(fptr);

	/* save.txt dosyasının içeriğinin ana(düzeltilebilir) dosyaya aktarılması */
	save_ptr = fopen("./.kal/save.txt", "r");
	if(isNull(save_ptr, "save.txt could not be opened! (Save)\n")) return;
	fptr = fopen(new_save.file_path, "w");
	if(isNull(fptr, "save object file could not be opened! (Save)\n")) return;
	while(fgets(line, 128, save_ptr) != NULL)
	{
		fprintf(fptr, "%s", line);
	}
	fclose(fptr);
	fclose(save_ptr);
	free(new_save.content);

	/* save.txt dosyasının silinmesi */
	sprintf(command, "rm -f ./.kal/save.txt");
	system(command);

	/* tracked_files.txt sıfırlanmalı */
	fptr = fopen("./.kal/tracked_files.txt", "w");
	fclose(fptr);

	/* en son yapılan kaydın(save) hash ini kullanabilmek için bir dosyaya kaydetme */
	fptr = fopen("./.kal/last_save_hash.txt", "w");
	if(isNull(fptr, "last_save_hash.txt could not be opened! (Save)\n")) return;
	fprintf(fptr, "last save hash: %s", new_save.hash);
	fclose(fptr);

	/* en son yapılan kaydın hash ini mevcut dalı bulup mevcut dala kaydetme */
	fptr = fopen(current_branch_path, "w");
	if(isNull(fptr, "current branch file could not be opened! (Save)\n")) return;
	fprintf(fptr, "%s", new_save.hash);
	fclose(fptr);

	/* kullıcıyı kayıt yapıldığına dair bilgilendirme */
	strncpy(short_hash, new_save.hash, 7);
	short_hash[7] = '\0';
	printf("[%s %s]", &(current_branch_path[16]), short_hash);
	printf("save succeffuly done\n");

    return;
}

void SetConfig(int argc, char** argv)
{
	FILE* kalconfig_ptr;
	FILE* temp_ptr;
	char* name;
	char* email;
	int i;
	struct passwd* pw;
	char config_file_path[128];
	int param_index;
	char line[128];
	char keyword[16];
	char command[32];

	/* kullanıcıya ait bilgileri (özellikle ev dizininin yolunu) alma */
	if((pw = getpwuid(getuid())) == NULL)
	{
		printf("getpwuid: no password entry\n");
	}

	/* ayar dosyasının yolunu oluşturma */
	if(strcmp(argv[2], "--global") == 0)
	{
		strcpy(config_file_path, pw->pw_dir);
		strcat(config_file_path, "/.kalconfig");
		param_index = 3;
	}
	else if(strcmp(argv[2], "--global") != 0)
	{
		strcpy(config_file_path, "./.kal/config");
		param_index = 2;
	}

	/* ayar dosyasını okuma, eğer dosya mevcut değilse dosyayı oluşturma */
	kalconfig_ptr = fopen(config_file_path, "r");
	if(kalconfig_ptr == NULL)
	{
		kalconfig_ptr = fopen(config_file_path, "w");
		fprintf(kalconfig_ptr, "[user]\n");
		fclose(kalconfig_ptr);
		kalconfig_ptr = fopen(config_file_path, "r");
	}

	/* girilen veri türü dosyada yoksa dosyaya kaydedilmesi */
	if(!inConfig(config_file_path, &(argv[param_index][5])))
	{
		fclose(kalconfig_ptr); /* dosya açma modunu r+ dan a+ ya almak için */
		kalconfig_ptr = fopen(config_file_path, "a+");
		if(strcmp(argv[param_index], "user.name") == 0)
		{
			/* kullanıcı adını parametreden değişkene atma ve yazdırma */
			name = (char*)malloc(sizeof(char) * strlen(argv[param_index + 1]));
			strcpy(name, argv[param_index + 1]);
			fprintf(kalconfig_ptr, "\tname = %s\n", name);
			free(name);
		}
		else if(strcmp(argv[param_index], "user.email") == 0)
		{
			/* email i parametreden değişkene atma ve yazdırma */
			email = (char*)malloc(sizeof(char) * strlen(argv[param_index + 1]));
			strcpy(email, argv[param_index + 1]);
			fprintf(kalconfig_ptr, "\temail = %s\n", email);
			free(email);
		}
		fclose(kalconfig_ptr);
		return;
	}

	/* temp_config.txt dosyasının oluşturulması */
	temp_ptr = fopen("./.kal/temp_config.txt", "w");
	if(temp_ptr == NULL)
	{
		printf("temp_config.txt file could not be generated! (SetConfig)\n");
		return;
	}

	/* eğer girilen veri türü dosyada varsa temp_config.txt ye yazma ve ana ayar dosyasına geri yazma */
	while(fgets(line, 128, kalconfig_ptr) != NULL)
	{
		if(strcmp(line, "[user]\n") == 0)
		{
			fprintf(temp_ptr, "%s", line);
			continue;
		}
		sscanf(line, "\t%s", keyword);
		if(strcmp(&(argv[param_index][5]), keyword) == 0)
		{
			/* temp_config.txt dosyasına yazılacak olan verinin tespit edilmesi */
			if(strcmp(argv[param_index], "user.name") == 0)
			{
				/* kullanıcı adını parametreden değişkene atma ve yazdırma */
				name = (char*)malloc(sizeof(char) * strlen(argv[param_index + 1]));
				strcpy(name, argv[param_index + 1]);
				fprintf(temp_ptr, "\tname = %s\n", name);
				free(name);
			}
			else if(strcmp(argv[param_index], "user.email") == 0)
			{
				/* email i parametreden değişkene atma ve yazdırma */
				email = (char*)malloc(sizeof(char) * strlen(argv[param_index + 1]));
				strcpy(email, argv[param_index + 1]);
				fprintf(temp_ptr, "\temail = %s\n", email);
				free(email);
			}
		}
		else
		{
			fprintf(temp_ptr, "%s", line);
		}
	}
	fclose(kalconfig_ptr);
	fclose(temp_ptr);

	/* temp_config.txt dosyasından ana ayar dosyasına geri yazma */
	kalconfig_ptr = fopen(config_file_path, "w");
	temp_ptr = fopen("./.kal/temp_config.txt", "r");
	while(fgets(line, 128, temp_ptr) != NULL)
	{
		fprintf(kalconfig_ptr, "%s", line);
	}
	fclose(temp_ptr);
	fclose(kalconfig_ptr);

	/* temp_config.txt dosyasını silme */
	sprintf(command, "rm -f ./.kal/%s", "temp_config.txt");
	system(command);
}

void SetDefaultBranchName(char* default_branch_name)
{
	FILE* config_fptr;
	struct passwd* pw;
	char config_file_path[128];

	/* kullanıcıya ait bilgileri (özellikle ev dizininin yolunu) alma */
	if((pw = getpwuid(getuid())) == NULL)
	{
		printf("getpwuid: no password entry\n");
	}

	/* ayar dosyasının yolunu hazırlama */
	sprintf(config_file_path, "%s/.kalconfig", pw->pw_dir);

	/* eğer küresel ayar dosyası mevcutsa ekleme yapma, mevcut değilse oluşturup yazma */
	config_fptr = fopen(config_file_path, "r");
	if(config_fptr == NULL)
	{
		config_fptr = fopen(config_file_path, "w");
		fprintf(config_fptr, "[init]\n\tdefaultBranch = %s\n", default_branch_name);
		fclose(config_fptr);
		return;
	}
	fclose(config_fptr);
	config_fptr = fopen(config_file_path, "a+");
	if(isNull(config_fptr, "global config file could not be generated! (SetDefaultBranchName)\n"))
		return;
	fprintf(config_fptr, "[init]\n\tdefaultBranch = %s\n", default_branch_name);
	fclose(config_fptr);
}

void GenerateBranch(int argc, char** argv)
{
	/* parametre olarak argc yi aldı ama kullanılmadığı için kaldırılabilir */
	int i;
	char file_path[64];
	char current_branch_path[64];
	char new_branch_path[64];
	char command[256];
	FILE* fptr;

	/* mevcut dalın hangi dalın yolunu alma */
	/* HEAD dosyası dallarla birlikte kayıtları da takip edebildiği zaman değişmeli */
	fptr = fopen("./.kal/HEAD", "r");
	if(isNull(fptr, "HEAD could not be opened! (GenereateBranch)\n")) 
		return;
	fscanf(fptr, "%*s %s", current_branch_path);
	fclose(fptr);

	/* yeni dalın yolunu oluşturma */
	sprintf(new_branch_path, "./.kal/branches/%s", argv[2]);

	/* yeni dalı oluşturma */
	sprintf(command, "cp %s %s", current_branch_path, new_branch_path);
	system(command);
}

void DeleteBranch(int argc, char** argv)
{
	int i;
	char command[128];
	char file_path[64];

	if(argc <= 3)
	{
		printf("There is no parameter given!\n");
		return;
	}

	for(i = 3; i < argc; i++)
	{
		/* eğer parametre olarak girilen dal adı mevcut değilse kullanıcı bilgilendirilmeli */
		if(!isInDir(argv[i], "./.kal/branches"))
		{
			printf("branch '%s' does not exist!\n", argv[i]);
			continue;
		}

		/* silinecek dal dosyasının yolunu alma */
		strcpy(file_path, "./.kal/branches/");
		strcat(file_path, argv[i]);

		/* dal dosyasını silme komutunu oluşturma ve çalıştırma */
		strcpy(command, "rm -f ");
		strcat(command, file_path);
		system(command);
	}
}

void Merge(char* to_branch, char* merge_save_message)
{
	return;
}

void ListBranches()
{
	DIR* dir;
	struct dirent* entry;
	FILE* fptr;
	char current_branch_name[32];

	/* mevcut dalın ismini öğrenme */
	fptr = fopen("./.kal/HEAD", "r");
	if(isNull(fptr, "HEAD could not be opened! (ListBranches)\n"))
		return;
	fscanf(fptr, "branch: ./.kal/branches/%s", current_branch_name);
	fclose(fptr);

	dir = opendir("./.kal/branches");
	if(isNull(dir, "branches directory could not be opened! (ListBranches)\n")) return;
	printf("branch(es):\n");
	while((entry = readdir(dir)) != NULL)
	{
		if((entry->d_name)[0] == '.') continue;
		if(strcmp(entry->d_name, current_branch_name) == 0)
			printf(GRN "\t%s\n" RESET, entry->d_name);
		else
			printf("\t%s\n", entry->d_name);
	}
}

void Checkout(char* branch_or_save_hash)
{
	FILE* fptr;
	FILE* content_fptr;
	DIR* dir;
	struct dirent* entry;
	DIR* save_dir;
	struct dirent* save_entry;
	int hash_lenght;
	char dir_name[3];
	char dir_path[18];
	char file_name[16];
	char file_path[57];
	char content_file_hash[41];
	int matching_hashes;
	char full_hash[41];
	char save_hash[41];
	char tree_hash[41];
	char line[128];
	char* compressed_content;
	int file_size;
	char command[256];

	/* parametrenin bir dal olup olmadığını bulmak için dallar klasöründe dolaşma ve
	eğer parametre bir dal ise */
	if(isInDir(branch_or_save_hash, "./.kal/branches"))
	{
		/* mevcut dalın yolunu güncelleme */
		fptr = fopen("./.kal/HEAD", "w");
		if(isNull(fptr, "HEAD could not be opened! (ChangeBranch)\n"))
			return;
		fprintf(fptr, "branch: ./.kal/branches/%s", branch_or_save_hash);
		fclose(fptr);

		/* geçiş yapılan daldaki kayda göre dosya ve klasörleri güncelleme */
		/* dalın bulunduğu kaydın hash ini alma */
		sprintf(file_path, "./.kal/branches/%s", branch_or_save_hash);
		fptr = fopen(file_path, "r");
		if(isNull(fptr, "the branch file could not be opened! (Checkout)\n"))
			return;
		fscanf(fptr, "%s", save_hash);
		fclose(fptr);

		/* kayıt dosyasının hash ini kullanarak kayıt dosyasının klasör ve dosya yollarını oluşturma */
		GenerateDirPath(dir_path, save_hash);
		GenerateFilePath(file_path, save_hash);

		/* save dosyasındaki tree nin hash ini alma */
		fptr = fopen(file_path, "r");
		if(isNull(fptr, "the save file could not be opened! (Checkout)\n"))
			return;
		fgets(line, 128, fptr);
		fgets(line, 128, fptr);
		sscanf(line, "tree %s\n", tree_hash);
		fclose(fptr);

		/* tree dosyası bulabilmek için yolu hazırlama */
		GenerateFilePath(file_path, tree_hash);

		/* gidilen kayıttaki tüm durumları dosyalara uygulamak için tree dosyasındaki durum hash lerini okuma */
		fptr = fopen(file_path, "r");
		if(isNull(fptr, "tree file could not be opened! (Checkout)\n"))
			return;
		fgets(line, 128, fptr);
		while(fgets(line, 128, fptr) != NULL)
		{
			sscanf(line, "%s -> %s\n", file_name, content_file_hash);

			/* durum hash lerini kullanarak sıkıştırılmış içeriğin olduğu dosya yolunu alma */
			GenerateFilePath(file_path, content_file_hash);

			/* sıkıştırılmış içeriği açarak gerçek içeriği elde etme ve dosyaya yazma */
			sprintf(command, "/bin/bash -c 'base64 -d %s | gunzip > ./%s'", file_path, file_name);
			system(command);
		}
		fclose(fptr);

		printf("Switched to branch '%s'\n", branch_or_save_hash);
	}
	/* parametre bir dal değilse bir kayıt hash i olup olmadığnı kontrol etme */
	else
	{
		/* parametre olarak verilen hash ten birden fazla var mı kontrol etme, varsa kullanıcıyı bilgilendirme */
		hash_lenght = strlen(branch_or_save_hash);
		/* buradaki hash uzunluğu kriteri daha sonradan değiştirilebilmeli */
		if(hash_lenght <= 6)
		{
			printf("given hash %s is too short!\n", branch_or_save_hash);
			return;
		}
		/* hash in eşleşip eşleşmediği kontrol edilecek olan dizinin adını oluşturma */
		dir_name[0] = branch_or_save_hash[0];
		dir_name[1] = branch_or_save_hash[1];
		dir_name[2] = '\0';
		matching_hashes = 0;
		dir = opendir("./.kal/objects");
		if(isNull(dir, "objects directory could not be opened! (Checkout)\n"))
			return;
		while((entry = readdir(dir)) != NULL )
		{
			/* objects klasöründe, hash in ilk 2 karakteri ile eşleşen klasörlerin içlerine bakma */
			if(strcmp(entry->d_name, dir_name) == 0)
			{
				GenerateDirPath(dir_path, branch_or_save_hash);
				save_dir = opendir(dir_path);
				if(isNull(save_dir, "save directory could not be opened! (Checkout)\n"))
					return;
				while((save_entry = readdir(save_dir)) != NULL)
				{
					if(strncmp(save_entry->d_name, &(branch_or_save_hash[2]), hash_lenght - 2) == 0)
					{
						/* tam hash i alma */
						strcpy(full_hash, dir_name);
						strcat(full_hash, save_entry->d_name);

						/* girilen hash in bir kayıt hash i olup olmadığını kontrol etme (kaldırılabilir) 
						bunun için önce kayıt dosyasının yolunu hazırlama */
						strcpy(file_path, "./.kal/objects/");
						strcat(file_path, dir_name);
						strcat(file_path, "/");
						strcat(file_path, save_entry->d_name);

						fptr = fopen(file_path, "r");
						if(isNull(fptr, "file could not be opened! (Checkout)\n"))
						{
							closedir(save_dir);
							return;
						}
						fgets(line, 128, fptr);
						if(strcmp(line, "save\n") != 0)
						{
							printf("the hash %s does not belong to a save file!\n", branch_or_save_hash);
							fclose(fptr);
							closedir(save_dir);
							return;
						}
						fclose(fptr);

						matching_hashes++;
					}
				}
				closedir(save_dir);
			}
		}
		closedir(dir);

		if(matching_hashes > 1)
		{
			printf("there are more than 1 mathing save with hash %s\n", branch_or_save_hash);
			return;
		}
		else if(matching_hashes == 1)
		{
			/* HEAD dosyasının içeriğini güncelleme */
			fptr = fopen("./.kal/HEAD", "w");
			if(isNull(fptr, "HEAD could not be opened! (Checkout)\n"))
				return;
			fprintf(fptr, "checkout: %s", full_hash);
			fclose(fptr);

			/* save dosyasındaki tree nin hash ini alma */
			fptr = fopen(file_path, "r");
			if(isNull(fptr, "save file could not be opened! (Checkout)\n"))
				return;
			fgets(line, 128, fptr);
			fgets(line, 128, fptr);
			sscanf(line, "tree %s\n", tree_hash);
			fclose(fptr);

			/* tree dosyası bulabilmek için yolu hazırlama */
			GenerateFilePath(file_path, tree_hash);

			/* gidilen kayıttaki tüm durumları dosyalara uygulamak için tree dosyasındaki durum hash lerini okuma */
			fptr = fopen(file_path, "r");
			if(isNull(fptr, "tree file could not be opened! (Checkout)\n"))
				return;
			fgets(line, 128, fptr);
			while(fgets(line, 128, fptr) != NULL)
			{
				sscanf(line, "%s -> %s\n", file_name, content_file_hash);

				/* durum hash lerini kullanarak sıkıştırılmış içeriğin olduğu dosya yolunu alma */
				GenerateFilePath(file_path, content_file_hash);

				/* sıkıştırılmış içeriği açarak gerçek içeriği elde etme ve dosyaya yazma */
				sprintf(command, "/bin/bash -c 'base64 -d %s | gunzip > ./%s'", file_path, file_name);
				system(command);
			}
			fclose(fptr);
		}
		/* eğer parametre hem dal hem de kayıt değilse kullanıcı bilgilendirilmeli */
		else
		{
			printf("there is no any branch called %s or save with hash %s\n", branch_or_save_hash, branch_or_save_hash);
			return;
		}
	}
}

void AmendSave(char* new_save_message)
{
	FILE* fptr;
	FILE* save_file_ptr;
	FILE* temp_file_ptr;
	DIR* objects_dir;
	DIR* save_object_dir;
	struct dirent* object_entry;
	struct dirent* save_object_entry;
	char last_save_hash[41];
	char save_file_path[57];
	char dir_name[3];
	char line[128];
	int i;
	char command[64];
	
	/* son yapılan kaydın(save) hash ini alma */
	fptr = fopen("./.kal/last_save_hash.txt", "r");
	if(isNull(fptr, "last_save_hash.txt could not be opened! (AmendSave)\n")) return;
	fscanf(fptr, "last save hash: %s", last_save_hash);
	fclose(fptr);

	/* son save dosyasının yolunu oluşturma */
	GenerateFilePath(save_file_path, last_save_hash);
	
	/* geçici bir dosya oluşturup içeriği güncellenmiş mesajla birlikte geçici mesaja yazdıktan sonra
	geçici dosyada oluşturulan yeni içeriği tekrar ana dosyaya aktarma ve geçici dosyayı silme */
	save_file_ptr = fopen(save_file_path, "r");
	if(isNull(save_file_ptr, "save file could not be opened! (AmendSave 1)\n")) return;
	temp_file_ptr = fopen("./.kal/temp.txt", "w");
	if(isNull(temp_file_ptr, "temp file could not be generated! (AmendSave)\n")) return;
	/* save dosyasının içeriği değişirse bu sayı değişebilir */
	for(i = 0; i < 7; i++)
	{
		fgets(line, 128, save_file_ptr);
		fprintf(temp_file_ptr, "%s", line);
	}
	fprintf(temp_file_ptr, "save message: %s", new_save_message);
	fclose(temp_file_ptr);
	fclose(save_file_ptr);

	save_file_ptr = fopen(save_file_path, "w");
	if(isNull(save_file_ptr, "save file could not be opened! (AmendSave 2)\n")) return;
	temp_file_ptr = fopen("./.kal/temp.txt", "r");
	if(isNull(temp_file_ptr, "temp file could not be opened! (AmendSave)\n")) return;
	while(fgets(line, 128, temp_file_ptr) != NULL)
	{
		fprintf(save_file_ptr, "%s", line);
	}
	fclose(temp_file_ptr);
	fclose(save_file_ptr);

	sprintf(command, "rm -f ./.kal/temp.txt");
	system(command);
}

void Reflog()
{
	FILE* log_file_ptr;
	char line[128];

	log_file_ptr = fopen("./.kal/commands_log.txt", "r");
	if(isNull(log_file_ptr, "commands_log.txt could not be opened! (RefLog)\n")) return;
	while(fgets(line, 128, log_file_ptr) != NULL)
	{
		printf("%s", line);
	}
}

void Log()
{
	FILE* fptr;
	FILE* branch_fptr;
	char save_hash[41];
	char parent_hash[41];
	char save_file_path[64];
	char branch_path[64];
	char branch_hash[41];
	char branch_name[16];
	char line[128];
	char author_name[32];
	char author_email[64];
	char date[64];
	char save_message[64];
	char c;
	int i;
	int index;
	DIR* dir;
	struct dirent* entry;
	int branch_on_hash_counter = 0;

	/* mevcut dalın adresini alma */
	fptr = fopen("./.kal/HEAD", "r");
	if(isNull(fptr, "HEAD could not be opened! (Log)\n"))
		return;
	fscanf(fptr, "branch: %s", branch_path);
	sscanf(branch_path, "./.kal/branches/%s", branch_name);
	fclose(fptr);

	/* mevcut dalı okuyarak en son yapılan kaydın hash ini alma */
	fptr = fopen(branch_path, "r");
	if(isNull(fptr, "current branch file could not be opened! (Log)\n"))
		return;
	fscanf(fptr, "%s", save_hash);
	fclose(fptr);

	/* son yapılan kayıt dosyasının yolunu oluşturma */
	GenerateFilePath(save_file_path, save_hash);

	/* kayıt dosyasından gösterilecek bilgileri alma */
	fptr = fopen(save_file_path, "r");
	if(isNull(fptr, "save file path could not be opened! (Log)\n"))
		return;
	fgets(line, 128, fptr);
	fgets(line, 128, fptr);
	fgets(line, 128, fptr);
	i = 7;
	index = 0;
	while((c = line[i++]) != '\n') parent_hash[index++] = c;
	parent_hash[index] = '\0';
	fgets(line, 128, fptr);
	i = 7;
	index = 0;
	while((c = line[i++]) != '|') author_name[index++] = c;
	author_name[index] = '\0';
	i++;
	index = 0;
	while((c = line[i++]) != '\n') author_email[index++] = c;
	author_email[index] = '\0';
	fgets(line, 128, fptr);
	fgets(line, 128, fptr);
	i = 10;
	index = 0;
	while((c = line[i++]) != '\0') date[index++] = c;
	date[index] = '\0';
	fgets(line, 128, fptr);
	fgets(line, 128, fptr);
	i = 14;
	index = 0;
	while((c = line[i++]) != '\0') save_message[index++] = c;
	save_message[index] = '\0';
	fclose(fptr);

	printf(YEL "save %s (" RESET, save_hash);
	printf(CYN "HEAD " RESET); printf(YEL "-> " RESET); printf(GRN "%s" RESET, branch_name); 
	
	dir = opendir("./.kal/branches");
	if(isNull(dir, "branches directory could not be opened! (Log)\n"))
		return;
	while((entry = readdir(dir)) != NULL)
	{
		if((entry->d_name)[0] == '.')
			continue;

		/* dallar klasöründeki tüm dalların dosya yollarını alma */
		sprintf(branch_path, "./.kal/branches/%s", entry->d_name);

		/* dalların dosya yollarını kullanarak hash lerini alma */
		branch_fptr = fopen(branch_path, "r");
		if(isNull(branch_fptr, "the branch file could not be opened! (Log)\n"))
			return;
		fscanf(branch_fptr, "%s", branch_hash);
		fclose(branch_fptr);

		/* eğer dalın bulunduğu kaydın hash i ile bilgileri gösterilecek kaydın hash i aynı ise dalı da gösterme */
		if(strcmp(save_hash, branch_hash) == 0 && strcmp(branch_name, entry->d_name) != 0)
		{
			printf(YEL ", " RESET); printf(GRN "%s" RESET, entry->d_name);
		}
	}
	closedir(dir);

	printf(YEL ")\n" RESET);
	printf("Author: %s<%s>\n", author_name, author_email);
	printf("Date:   %s\n", date);
	printf("\t%s\n\n", save_message);

	strcpy(save_hash, parent_hash);

	/* diğer kayıtların bilgilerini yazdırma */
	while(strcmp(save_hash, "NONE") != 0)
	{
		/* sıradaki kayıt dosyasının yolunu oluşturma */
		GenerateFilePath(save_file_path, save_hash);

		/* kayıt dosyasından gösterilecek bilgileri alma */
		fptr = fopen(save_file_path, "r");
		if(isNull(fptr, "save file path could not be opened! (Log)\n"))
			return;
		fgets(line, 128, fptr);
		fgets(line, 128, fptr);
		fgets(line, 128, fptr);
		i = 7;
		index = 0;
		while((c = line[i++]) != '\n') parent_hash[index++] = c;
		parent_hash[index] = '\0';
		fgets(line, 128, fptr);
		i = 7;
		index = 0;
		while((c = line[i++]) != '|') author_name[index++] = c;
		author_name[index] = '\0';
		i++;
		index = 0;
		while((c = line[i++]) != '\n') author_email[index++] = c;
		author_email[index] = '\0';
		fgets(line, 128, fptr);
		fgets(line, 128, fptr);
		i = 10;
		index = 0;
		while((c = line[i++]) != '\0') date[index++] = c;
		date[index] = '\0';
		fgets(line, 128, fptr);
		fgets(line, 128, fptr);
		i = 14;
		index = 0;
		while((c = line[i++]) != '\0') save_message[index++] = c;
		save_message[index] = '\0';

		/* mevcut kayıt üzerinde herhangi bir dal var mı kontrol etme, varsa yazdırma */
		dir = opendir("./.kal/branches");
		if(isNull(dir, "branches directory could not be opened! (Log)\n"))
			return;
		while((entry = readdir(dir)) != NULL)
		{
			if((entry->d_name)[0] == '.')
				continue;

			/* dallar klasöründeki tüm dalların dosya yollarını alma */
			sprintf(branch_path, "./.kal/branches/%s", entry->d_name);

			/* dalların dosya yollarını kullanarak hash lerini alma */
			branch_fptr = fopen(branch_path, "r");
			if(isNull(branch_fptr, "the branch file could not be opened! (Log)\n"))
				return;
			fscanf(branch_fptr, "%s", branch_hash);
			fclose(branch_fptr);

			/* eğer dalın bulunduğu kaydın hash i ile bilgileri gösterilecek kaydın hash i sayacı 1 arttırma
			 (virgül ile yazdırabilmek için böyle yapıldı, verimlileştirilebilir) */
			if(strcmp(save_hash, branch_hash) == 0)
				branch_on_hash_counter++;
		}
		closedir(dir);

		/* daha verimli hale getirilebilir */
		if(branch_on_hash_counter == 0)
			printf(YEL "save %s\n" RESET, save_hash);
		else
			printf(YEL "save %s (" RESET, save_hash);

		/* sayaç ile mevcut kayıt üzerinde kaç tane dalın olduğunu aldıktan sonra kayıt üzerindeki dal(lar)ı gösterme */
		dir = opendir("./.kal/branches");
		if(isNull(dir, "branches directory could not be opened! (Log)\n"))
			return;
		while((entry = readdir(dir)) != NULL)
		{
			if((entry->d_name)[0] == '.')
				continue;

			/* dallar klasöründeki tüm dalların dosya yollarını alma */
			sprintf(branch_path, "./.kal/branches/%s", entry->d_name);

			/* dalların dosya yollarını kullanarak hash lerini alma */
			branch_fptr = fopen(branch_path, "r");
			if(isNull(branch_fptr, "the branch file could not be opened! (Log)\n"))
				return;
			fscanf(branch_fptr, "%s", branch_hash);
			fclose(branch_fptr);

			/* eğer dalın bulunduğu kaydın hash i ile bilgileri gösterilecek kaydın hash i aynı ise dalı da gösterme */
			if(strcmp(save_hash, branch_hash) == 0)
			{
				if(branch_on_hash_counter > 1)
				{
					printf(GRN "%s" RESET, entry->d_name); printf(YEL ", " RESET);
				}
				else
				{
					printf(GRN "%s" RESET, entry->d_name); printf(YEL ")\n" RESET);
				}
				branch_on_hash_counter--;
			}
		}
		closedir(dir);

		printf("Author: %s<%s>\n", author_name, author_email);
		printf("Date:   %s\n", date);
		printf("\t%s\n\n", save_message);

		strcpy(save_hash, parent_hash);
		
		fclose(fptr);
	}
}

void Log_OneLine()
{
	FILE* fptr;
	char save_hash[41];
	char min_save_hash[8]; /* bu uzunluk ayarlardaki veriye göre değişebilir */
	char parent_hash[41];
	char save_file_path[64];
	char branch_path[64];
	char branch_name[16];
	char line[128];
	char save_message[64];
	char c;
	int i, j;
	int index;

	/* mevcut dalın adresini alma */
	fptr = fopen("./.kal/HEAD", "r");
	if(isNull(fptr, "HEAD could not be opened! (Log)\n"))
		return;
	fscanf(fptr, "branch: %s", branch_path);
	sscanf(branch_path, "./.kal/branches/%s", branch_name);
	fclose(fptr);

	/* mevcut dalı okuyarak en son yapılan kaydın hash ini alma */
	fptr = fopen(branch_path, "r");
	if(isNull(fptr, "current branch file could not be opened! (Log)\n"))
		return;
	fscanf(fptr, "%s", save_hash);
	fclose(fptr);

	/* son yapılan kayıt dosyasının yolunu oluşturma */
	GenerateFilePath(save_file_path, save_hash);

	/* kayıt dosyasından gösterilecek bilgileri alma */
	fptr = fopen(save_file_path, "r");
	if(isNull(fptr, "save file path could not be opened! (Log)\n"))
		return;
	for(j = 0; j < 8; j++)
	{
		fgets(line, 128, fptr);
		if(j == 2)
		{
			i = 7;
			index = 0;
			while((c = line[i++]) != '\n') parent_hash[index++] = c;
			parent_hash[index] = '\0';
		}
	}
	i = 14;
	index = 0;
	while((c = line[i++]) != '\0') save_message[index++] = c;
	save_message[index] = '\0';

	strncpy(min_save_hash, save_hash, 7);
	min_save_hash[7] = '\0';

	printf(YEL "%s (" RESET, min_save_hash);
	printf(CYN "HEAD " RESET); printf(YEL "-> " RESET); printf(GRN "%s" RESET, branch_name); printf(YEL ") " RESET);
	printf("%s\n", save_message);

	strcpy(save_hash, parent_hash);

	/* diğer kayıtların bilgilerini yazdırma */
	while(strcmp(save_hash, "NONE") != 0)
	{
		/* sıradaki kayıt dosyasının yolunu oluşturma */
		GenerateFilePath(save_file_path, save_hash);
		
		/* kayıt dosyasından gösterilecek bilgileri alma */
		fptr = fopen(save_file_path, "r");
		if(isNull(fptr, "save file path could not be opened! (Log)\n"))
			return;
		for(j = 0; j < 8; j++)
		{
			fgets(line, 128, fptr);
			if(j == 2)
			{
				i = 7;
				index = 0;
				while((c = line[i++]) != '\n') parent_hash[index++] = c;
				parent_hash[index] = '\0';
			}
		}
		i = 14;
		index = 0;
		while((c = line[i++]) != '\0') save_message[index++] = c;
		save_message[index] = '\0';
	
		strncpy(min_save_hash, save_hash, 7);
		min_save_hash[7] = '\0';
	
		printf(YEL "%s " RESET, min_save_hash);
		printf("%s", save_message);
	
		strcpy(save_hash, parent_hash);
	}
}

void ListClean()
{
	DIR* dir;
	struct dirent* entry;

	dir = opendir(".");
	if(isNull(dir, "current directory could not be opened! (ListClean)\n"))
		return;
	while((entry = readdir(dir)) != NULL)
	{
		if(!isInTrackedFiles(entry->d_name))
		{
			printf("Would remove ");
			printf(RED "%s\n" RESET, entry->d_name);
		}
	}
	closedir(dir);
}

void RemoveClean()
{
	DIR* dir;
	struct dirent* entry;
	char command[512];

	dir = opendir(".");
	if(isNull(dir, "current directory could not be opened! (ListClean)\n"))
		return;
	while((entry = readdir(dir)) != NULL)
	{
		if(!isInTrackedFiles(entry->d_name))
		{
			printf("Removing ");
			printf(RED "%s\n" RESET, entry->d_name);
			sprintf(command, "rm -f %s", entry->d_name);
			system(command);
		}
	}
	closedir(dir);
}

void AddLog(int argc, char** argv)
{
	FILE* log_file_ptr;
	int i;
	int line_num = 0;
	char line[128];
	char last_command[128];

	/* eğer 'kal' başlatılmadıysa komut geçmişini kaydetmesin */
	if(stat("./.kal", &st) == -1) return;

	/* kayıt dosyasındaki satır sayısını öğrenme */
	log_file_ptr = fopen("./.kal/commands_log.txt", "r");
	if(log_file_ptr == NULL) line_num = 0;
	else
	{
		while(fgets(line, 128, log_file_ptr) != NULL)
			line_num++;
		fclose(log_file_ptr);
	}
	
	/* en son girilen komutu oluşturma */
	strcpy(last_command, argv[0]);
	for(i = 1; i < argc; i++)
	{
		strcat(last_command, " ");
		strcat(last_command, argv[i]);
	}

	/* en son girilen komutu commands_log.txt dosyasına ekleme */
	log_file_ptr = fopen("./.kal/commands_log.txt", "a+");
	if(isNull(log_file_ptr, "commands_log.txt could not be opened (AddLog)\n")) return;
	fprintf(log_file_ptr, "%d: %s\n", line_num + 1, last_command);
	fclose(log_file_ptr);
}

void calculate_sha1(char* content, char* output_hash) 
{
	int i;
    unsigned char hash[SHA_DIGEST_LENGTH]; 
    SHA1((unsigned char*)content, strlen(content), hash);
    /* Binary hash'i hex string'e çevir */
    for(i = 0; i < SHA_DIGEST_LENGTH; i++) {
        sprintf(output_hash + (i * 2), "%02x", hash[i]);
    }
    output_hash[40] = '\0';
}

void GetContent(char* file_path, char** getContentPtr)
{
	FILE* fptr;
	int file_size;
	char c;
	int contentIndex = 0;

	/* dosyanın boyutunu öğrenme */
	file_size = GetFileSize(file_path);
	
	/* dosyanın içeriğini almak için dosyayı açma ve içeriği alma */
	fptr = fopen(file_path, "r");
	if(fptr == NULL)
	{
		printf("%s could not be opened to get content!\n", file_path);
		return;
	}

	*getContentPtr = (char*)malloc(sizeof(char) * file_size + 1);

	while((c = getc(fptr)) != EOF)
	{
		(*getContentPtr)[contentIndex++] = c;
	}
	(*getContentPtr)[contentIndex] = '\0';
	fclose(fptr);
}

int GetFileSize(char* file_path)
{
	int file_size;
	FILE* fptr;

	fptr = fopen(file_path, "r");	
	if(isNull(fptr, "the file could not be opened (GetFileSize)\n"))
		return 0;
	/* dosyanın boyutunu öğrenme */
	fseek(fptr, 0, SEEK_END);
	file_size = ftell(fptr);
	fclose(fptr);

	return file_size;
}

int isInDir(char* file_name, char* dir_path)
{
	DIR* dir;
	struct dirent* entry;

	dir = opendir(dir_path);
	if(dir == NULL)
	{
		printf("directory could not be opened! (isInDir)\n");
		return 0;
	}

	while((entry = readdir(dir)) != NULL)
	{
		if(strcmp(entry->d_name, file_name) == 0) return 1;
	}
	return 0;
}

int isInTrackedFiles(char* file_name)
{
	FILE* fptr;
	char line[128];
	char name[32];
	char hash[48];
	
	fptr = fopen("./.kal/tracked_files.txt", "r");
	if(fptr == NULL)
	{
		printf("tracked_files.txt could not be opened! (isInTrackedFiles)\n");
		return 0;
	}

	while(fgets(line, 128, fptr) != NULL)
	{
		sscanf(line, "%s -> %s", name, hash);
		if(strcmp(name, file_name) == 0) return 1;
	}
	return 0;
}

int inArray(int argc, char** str_array, char* str)
{
	int i;

	for(i = 0; i < argc; i++)
	{
		if(strcmp(str_array[i], str) == 0) return 1;
	}
	return 0;
}

int inConfig(char* config_file_path, char* keyword_param)
{
	FILE* kalconfig_ptr;
	char line[128];
	char keyword[16];

	kalconfig_ptr = fopen(config_file_path, "r");
	if(kalconfig_ptr == NULL)
	{
		printf("config file could not be opened! (inConfig)\n");
		return 0;
	}

	while(fgets(line, 128, kalconfig_ptr) != NULL)
	{
		sscanf(line, "\t%s", keyword);
		if(strcmp(keyword, keyword_param) == 0) 
		{
			fclose(kalconfig_ptr);
			return 1;
		}
	}
	fclose(kalconfig_ptr);
	return 0;
}

void PrintConfigWarning()
{
	printf("Author identity unknown\n\n");
	printf("*** Please tell me who you are.\n\n");
	printf("Run\n\n");
	printf("  kal config --global user.name \"Your Name\"\n");
	printf("  kal config --global user.email \"you@example.com\"\n\n");
	printf("to set your account's default identity.\n");
	printf("Omit --global to set the identity only in this repository.\n\n");
}

void PrintTrackedFiles()
{
	FILE* fptr;
	char line[128];

	fptr = fopen("./.kal/tracked_files.txt", "r");
	if(isNull(fptr, "tracked_files.txt could not be opened! (PrintTrackedFiles)\n")) return;
	while(fgets(line, 128, fptr) != NULL)
	{
		printf("%s", line);
	}
	fclose(fptr);
}

/* void* ptr kısmı düzeltilebilir */
int isNull(void* ptr, char* message)
{
	if(ptr == NULL)
	{
		printf("%s\n", message);
		return 1;
	}
	return 0;
}

void FindMergeBase(char* current_branch_path, char* target_branch_path, char* merge_base_hash)
{
	FILE* curr_branch_fptr;
	FILE* target_branch_fptr;
	FILE* fptr;
	char curr_branch_hash[41];
	char last_target_branch_hash[41];
	char buff_target_branch_hash[41];
	char line[128];
	int i;
	int index;
	char save_file_path[57];
	char c;

	/* mevcut dalın başlangıç hash ini alma */
	curr_branch_fptr = fopen(current_branch_path, "r");
	if(isNull(curr_branch_fptr, "current branch file could not be opened (FindMergeBase)\n"))
		return;
	fscanf(curr_branch_fptr, "%s", curr_branch_hash);
	fclose(curr_branch_fptr);

	/* hedef dalın başlangıç hash ini alma */
	target_branch_fptr = fopen(target_branch_path, "r");
	if(isNull(target_branch_fptr, "target branch file could not be opened! (FindMergeBase)\n"))
		return;
	fscanf(target_branch_fptr, "%s", buff_target_branch_hash);
	fclose(target_branch_fptr);

	/* hedef dalın tüm hash lerini alarak mevcut dalın mevcut hash i ile karşılaştırarak ortak atayı bulma */
	while(strcmp(buff_target_branch_hash, "NONE") != 0)
	{
		strcpy(last_target_branch_hash, buff_target_branch_hash);
		/* eğer hedef daldan alınan güncel hash mevcut dalda varsa ortak ata bu hash dir */
		if(isOnBranch(current_branch_path, last_target_branch_hash))
		{
			strcpy(merge_base_hash, last_target_branch_hash);
			return;
		}

		/* eğer güncel mevcut hash ortak ata hash i değilse hedef daldaki bir önceki hash e bak */
		strcpy(save_file_path, "./.kal/objects/");
		strncat(save_file_path, last_target_branch_hash, 2);
		save_file_path[17] = '\0';
		strcat(save_file_path, "/");
		strcat(save_file_path, &(last_target_branch_hash[2]));

		fptr = fopen(save_file_path, "r");
		if(isNull(fptr, "save file could not be opened! (FindMergeBase)\n"))
			return;
		fgets(line, 128, target_branch_fptr);
		fgets(line, 128, target_branch_fptr);
		fgets(line, 128, target_branch_fptr);
		i = 7;
		index = 0;
		while((c = getc(target_branch_fptr)) != '\n')
			buff_target_branch_hash[index++] = c;
		buff_target_branch_hash[index] = '\0';
		fclose(fptr);
	}
}

int isOnBranch(char* check_branch_path, char* check_hash)
{
	FILE* fptr;
	char hash[41];
	char last_hash[41];
	char line[128];
	char file_path[57];
	int i;
	int index;
	char c;

	/* kontrol edilecek dalın ilk hash ini alma */
	fptr = fopen(check_branch_path, "r");
	if(isNull(fptr, "branch file could not be opened to check the hash! (isOnHash)\n"))
		return;
	fscanf(fptr, "%s", hash);
	fclose(fptr);
	/* kontrol edilecek dalın ilk hash ini kullanarak kayıt dosyalarından diğer hash leri alma */
	while(strcmp(hash, "NONE") != 0)
	{
		strcpy(last_hash, hash);
		if(strcmp(check_hash, last_hash) == 0)
			return 1;
		strcpy(file_path, "./.kal/objects/");
		strncat(file_path, last_hash, 2);
		file_path[17] = '\0';
		strcat(file_path, "/");
		strcat(file_path, &(last_hash[2]));

		fptr = fopen(file_path, "r");
		if(isNull(fptr, "save file could not be opened! (isOnBranch)\n"))
			return;
		fgets(line, 128, fptr);
		fgets(line, 128, fptr);
		fgets(line, 128, fptr);
		i = 7;
		index = 0;
		while((c = getc(fptr)) != '\n')
			hash[index++] = c;
		hash[index] = '\0';
		fclose(fptr);
	}
	return 0;
}

int isNameInTree(char* tracked_file_name, char* search_tree_path)
{
	FILE* search_tree_fptr;
	char file_name[32];
	char file_hash[41];
	char line[128];

	/* parametre olarak gelen dosya adı ve hash ini ortak ata tree dosyasındaki tüm dosyalarla karşılaştırarak 
	dosya adı - hash çiftinin takip edilen dosyada olup olmadığını bulma, bu sayede */
	search_tree_fptr = fopen(search_tree_path, "r");
	if(isNull(search_tree_fptr, "base tree file could not be opened! (isInBaseTree)\n"))
		return;
	fgets(line, 128, search_tree_fptr); /* ilk satır gerekli olmadığı için */
	while(fgets(line, 128, search_tree_fptr) != NULL)
	{
		sscanf(line, "%s -> %s\n", file_name, file_hash);
		if(strcmp(tracked_file_name, file_name) == 0)
		{
			fclose(search_tree_fptr);
			return 1;
		}
	}
	fclose(search_tree_fptr);
	return 0;
}

int isTrackInTree(char* tracked_file_line, char* search_tree_path)
{
	FILE* search_tree_fptr;
	char file_name[32];
	char file_hash[41];
	char tracked_file_name[32];
	char tracked_file_hash[41];
	char line[128];

	/* parametre olarak gelen satırdan dosya adını ve dosya hash ini alma */
	sscanf(tracked_file_line, "%s -> %s\n", tracked_file_name, tracked_file_hash);

	/* parametre olarak gelen dosya adı ve hash ini ortak ata tree dosyasındaki tüm dosyalarla karşılaştırarak 
	dosya adı - hash çiftinin takip edilen dosyada olup olmadığını bulma, bu sayede */
	search_tree_fptr = fopen(search_tree_path, "r");
	if(isNull(search_tree_fptr, "base tree file could not be opened! (isInBaseTree)\n"))
		return;
	fgets(line, 128, search_tree_fptr); /* ilk satır gerekli olmadığı için */
	while(fgets(line, 128, search_tree_fptr) != NULL)
	{
		sscanf(line, "%s -> %s\n", file_name, file_hash);
		if(strcmp(tracked_file_name, file_name) == 0 && strcmp(tracked_file_hash, file_hash) == 0)
		{
			fclose(search_tree_fptr);
			return 1;
		}
	}
	fclose(search_tree_fptr);
	return 0;
}

int isInIgnoreFile(char* file_name)
{
	FILE* ignore_fptr;
	char line[128];
	char ignore_file_name[32];

	ignore_fptr = fopen("./.kalignore", "r");
	if(isNull(ignore_fptr, ".kalignore file could not be opened! (isInIgnoreFile)\n"));
		return;
	while(fgets(line, 128, ignore_fptr) != NULL)
	{
		sscanf(line, "%s\n", ignore_file_name);
		if(strcmp(ignore_file_name, file_name) == 0)
		{
			fclose(ignore_fptr);
			return 1;
		}
	}
	fclose(ignore_fptr);
	return 0;
}

void GenerateFilePath(char* path, char* hash)
{
	strcpy(path, "./.kal/objects/");
	strncat(path, hash, 2);
	path[17] = '\0';
	strcat(path, "/");
	strcat(path, &(hash[2]));
}

void GenerateDirPath(char* path, char* hash)
{
	strcpy(path, "./.kal/objects/");
	strncat(path, hash, 2);
	path[17] = '\0';
}

struct dirent* GetEntryFromName(char* entry_name)
{
	DIR* dir;
	struct dirent* entry;

	dir = opendir(".");
	if(isNull(dir, "current directory could not be opened! (GetEntryFromName)\n"))
		return;
	while((entry = readdir(dir)) != NULL)
	{
		if(strcmp(entry->d_name, entry_name) == 0)
		{
			closedir(dir);
			return entry;
		}
	}
	closedir(dir);
	return NULL;
}

void GetTrackingFile(char* dir_path, FILE* tracking_fptr)
{
	FILE* fptr;
	DIR* dir;
	struct dirent* entry;
	char entry_path[PATH_MAX];
	blob_t new_blob;
	tree_t new_tree;
	char tree_file_path[512];
	char command[1024];

	fprintf(tracking_fptr, "tree\n");

	dir = opendir(dir_path);
	if(isNull(dir, "the directory could not be opened! (GetTrackingFile)\n"))
		return;
	while((entry = readdir(dir)) != NULL)
	{
		if((entry->d_name)[0] == '.')
			continue;

		/* eğer içerik bir klasörse */
		if(entry->d_type == 4)
		{
			/* klasör yolunun bulunup bulunamadığı kontrol edilmeli
			(iç içe olan şeyleri doğru bulamadığı için farklı bir yöntem kullanıldı) */
			realpath(entry->d_name, entry_path);
			sprintf(entry_path, "%s/%s", dir_path, entry->d_name);
			
			/* geçici bir tree dosyası oluşturmak için tree dosyasının yolunu oluşturma */
			sprintf(tree_file_path, "./.kal/%s_tracked_files.txt", entry->d_name);

			fptr = fopen(tree_file_path, "w");
			GetTrackingFile(entry_path, fptr);
			fclose(fptr);

			GetContent(tree_file_path, &(new_tree.content));
			calculate_sha1(new_tree.content, new_tree.hash);

			fprintf(tracking_fptr, "%s -> %s\n", entry->d_name, new_tree.hash);

			/* klasörün hash ini kullanarak "objects" klasöründe bir klasör ve bir dosya oluşturma ve 
			geçici tree dosyasının içeriğini kalıcı olana aktarma */
			GenerateDirPath(new_tree.dir_path, new_tree.hash);
			GenerateFilePath(new_tree.file_path, new_tree.hash);

			/* hash in ilk 2 harfini kullanarak klasör oluşturulmalı */
			if(stat(new_tree.dir_path, &st) == -1)
			{
				mkdir(new_tree.dir_path, 0700);
			}

			/* o klasörün içine hash in geri kalanını kullanan dosya oluşturulmalı */
			fptr = fopen(new_tree.file_path, "w");
			fclose(fptr);

			/* o dosyanın içine içerik sıkıştırılarak yazılmalı */
			sprintf(command, "gzip -c %s | base64 > %s", tree_file_path, new_tree.file_path);
			system(command);

			sprintf(command, "rm -f %s", tree_file_path);
			system(command);
		}
		/* eğer içerik bir dosya ise doğrudan hash ini alarak dosyaya yazdır */
		else if(entry->d_type == 8)
		{
			/* klasör yolunun bulunup bulunamadığı kontrol edilmeli */
			realpath(entry->d_name, entry_path);
			
			/* realpath fonksiyonu üsste nedenini bilmediğim bir şekilde doğru çalışmadığı için yolu hazırlama */
			sprintf(entry_path, "%s/%s", dir_path, entry->d_name);

			GetContent(entry_path, &(new_blob.content));
			calculate_sha1(new_blob.content, new_blob.hash);
			fprintf(tracking_fptr, "%s -> %s\n", entry->d_name, new_blob.hash);

			/* dosyanın hash ini kullanarak dosyaya ait klasör ve dosya oluşturularak sıkıştırılmış içeriği dosyaya aktarma */
			GenerateDirPath(new_blob.dir_path, new_blob.hash);
			GenerateFilePath(new_blob.file_path, new_blob.hash);

			/* hash in ilk 2 harfini kullanarak klasör oluşturulmalı */
			if(stat(new_blob.dir_path, &st) == -1)
			{
				mkdir(new_blob.dir_path, 0700);
			}

			/* o klasörün içine hash in geri kalanını kullanan dosya oluşturulmalı */
			fptr = fopen(new_blob.file_path, "w");
			fclose(fptr);

			/* o dosyanın içine içerik sıkıştırılarak yazılmalı */
			sprintf(command, "gzip -c %s | base64 > %s", entry_path, new_blob.file_path);
			system(command);
		}
		/* eğer içerik dosya da klasör de değilse kullanıcıyı bilgilendirme (değiştirilebilir) */
		else
		{
			printf("entry %s is neither a regular file nor a directory!\n", entry->d_name);
			return;
		}
	}
	closedir(dir);
}

void JustGrep(char* keyword)
{
	FILE* tracked_files_txt_ptr;
	FILE* grep_files_txt_ptr;
	FILE* fptr;
	char line[128];
	char file_name[32];
	char file_hash[41];
	blob_t tracked_blob;
	int line_num;
	int keyword_index;
	int keyword_length;
	int offset;
	int match_count;
	int i;
	char command[128];
	char c;
	save_t save_obj;
	tree_t tree_obj;
	char current_branch_path[64];

	keyword_length = strlen(keyword);

	/* içeriğinde arama yapılacak dosyaların ve hash lerini bir dosyaya aktarma */
	grep_files_txt_ptr = fopen("./.kal/grep_files.txt", "w");
	if(isNull(grep_files_txt_ptr, "grep_files.txt could not be generated! (JustGrep)\n"))
		return;
	/* tracked_files.txt dosyasının içeriğini kopyalama */
	tracked_files_txt_ptr = fopen("./.kal/tracked_files.txt", "r");
	if(isNull(tracked_files_txt_ptr, "tracked_files.txt could not be opened! (JustGrep)\n"))
		return;
	while(fgets(line, 128, tracked_files_txt_ptr) != NULL)
	{
		fprintf(grep_files_txt_ptr, "%s", line);
	}
	fclose(tracked_files_txt_ptr);
	fclose(grep_files_txt_ptr);

	/* "objects" klasöründe "tree" ile başlayan dosyaların içeriklerini aktarma */
	/* son yapılan kaydın hash ini alma */
	fptr = fopen("./.kal/HEAD", "r");
	if(isNull(fptr, "HEAD could not be opened! (JustGrep)\n"))
		return;
	fscanf(fptr, "branch: %s", current_branch_path);
	fclose(fptr);

	fptr = fopen(current_branch_path, "r");
	if(isNull(fptr, "current branch file could not be opened! (JustGrep)\n"))
		return;
	fscanf(fptr, "%s", save_obj.hash);
	fclose(fptr);

	/* önceki kayıtlara giderek onların da tree dosyalarına erişmeyi sağlayan döngü */
	while(strcmp(save_obj.hash, "NONE") != 0 && GetFileSize(current_branch_path) != 0)
	{
		/* son yapılan kaydın hash ini kullanarak son tree ye erişme */
		GenerateFilePath(save_obj.file_path, save_obj.hash);

		/* save dosyasını açarak kayıda ait olan tree dosyasının hash ini ve bir önceki kaydın hash ini alma */
		fptr = fopen(save_obj.file_path, "r");
		if(isNull(fptr, "save file could not be opened! (JustGrep)\n"))
			return;
		fgets(line, 128, fptr);
		fgets(line, 128, fptr);
		sscanf(line, "tree %s\n", tree_obj.hash);
		fgets(line, 128, fptr);
		sscanf(line, "parent %s\n", save_obj.hash);
		fclose(fptr);

		GenerateFilePath(tree_obj.file_path, tree_obj.hash);

		/* son tree dosyasının içeriğini (ilk satır hariç) grep_files.txt dosyasına aktarma */
		grep_files_txt_ptr = fopen("./.kal/grep_files.txt", "a+");
		if(isNull(grep_files_txt_ptr, "grep_files.txt could not be opened! (JustGrep)\n"))
			return;
		fptr = fopen(tree_obj.file_path, "r");
		if(isNull(fptr, "tree file could not be opened! (JustGrep)\n"))
			return;
		fgets(line, 128, fptr);
		while(fgets(line, 128, fptr) != NULL)
		{
			fprintf(grep_files_txt_ptr, "%s", line);
		}
		fclose(fptr);
		fclose(grep_files_txt_ptr);
	}

	/* içeriği taranacak dosylar ve hash leri belirlendikten sonra dosyaların içeriklerinde tarama yapma */
	grep_files_txt_ptr = fopen("./.kal/grep_files.txt", "r");
	if(isNull(grep_files_txt_ptr, "grep_files.txt could not be opened! (JustGrep)\n"))
		return;
	while(fgets(line, 128, grep_files_txt_ptr) != NULL)
	{
		/* tracked_files.txt dosyasını satır satır okuma ve güncel olarak takip edilen dosyaların (klasörler de eklenmeli)
		isimlerini ve hash lerini alma */
		sscanf(line, "%s -> %s\n", file_name, file_hash);
		
		/* dosyaların hash lerini kullanarak sıkıştırılan içeriklerin dosya yolunu elde etme */
		GenerateFilePath(tracked_blob.file_path, file_hash);

		/* sıkıştırılan içeriklerin tutulduğu dosyanın yolunu kullanarak içeriği açarak geçici dosyaya aktarma */
		sprintf(command, "/bin/bash -c 'base64 -d %s | gunzip > ./.kal/temp_content_file.txt'", tracked_blob.file_path);
		system(command);

		/* açık içeriğin aktarıldığı geçici dosya açılarak eşleşme var mı kontrol etme */
		fptr = fopen("./.kal/temp_content_file.txt", "r");
		if(isNull(fptr, "temp_content_file.txt could not be opened! (JustGrep)\n"))
			return;
		line_num = 1;
		keyword_index = 0;
		offset = 0;
		match_count = 0;
		i = 0;
		while(c = getc(fptr))
		{
			offset++;
			if(c == '\n' || c == EOF)
			{
				if(match_count != 0)
				{
					fseek(fptr, -offset, SEEK_CUR);
					printf(MAG "%s" RESET, file_name); printf(CYN ":" RESET);
					keyword_index = 0;
					i = 0;
					while(0 < match_count || (c != '\n' && c != EOF)) /* kontrol edilmeli */
					{
						c = getc(fptr);

						if(c == keyword[keyword_index])
						{
							keyword_index++;
						}
						else if(c == EOF)
						{
							strnprint(keyword, keyword_index);
						}
						else
						{
							strnprint(keyword, keyword_index);
							printf("%c", c);
							keyword_index = 0;
						}

						if(keyword_index == keyword_length)
						{
							match_count--;
							printf(RED "%s" RESET, keyword);
							keyword_index = 0;
						}
					}
					keyword_index = 0;
					if(c == EOF)
					{
						break;
					} 
				}
				else
				{
					keyword_index = 0;
				}
				line_num++;
				offset = 0;		
				if(c == EOF) break;
			}
			else if(c == keyword[keyword_index])
			{
				keyword_index++;
			}
			else
			{
				keyword_index = 0;
			}

			if(keyword_index == keyword_length)
			{
				match_count++;
			}
		}
		fclose(fptr);
	}
	fclose(grep_files_txt_ptr);

	/* silinecek dosyaların silinmesi */
	sprintf(command, "rm -f ./.kal/temp_content_file.txt");
	system(command);
	sprintf(command, "rm -f ./.kal/grep_files.txt");
	system(command);
}

void NumGrep(char* keyword)
{
	FILE* tracked_files_txt_ptr;
	FILE* grep_files_txt_ptr;
	FILE* fptr;
	char line[128];
	char file_name[32];
	char file_hash[41];
	blob_t tracked_blob;
	int line_num;
	int keyword_index;
	int keyword_length;
	int offset;
	int match_count;
	int i;
	char command[128];
	char c;
	save_t save_obj;
	tree_t tree_obj;
	char current_branch_path[64];

	keyword_length = strlen(keyword);

	/* içeriğinde arama yapılacak dosyaların ve hash lerini bir dosyaya aktarma */
	grep_files_txt_ptr = fopen("./.kal/grep_files.txt", "w");
	if(isNull(grep_files_txt_ptr, "grep_files.txt could not be generated! (JustGrep)\n"))
		return;
	/* tracked_files.txt dosyasının içeriğini kopyalama */
	tracked_files_txt_ptr = fopen("./.kal/tracked_files.txt", "r");
	if(isNull(tracked_files_txt_ptr, "tracked_files.txt could not be opened! (JustGrep)\n"))
		return;
	while(fgets(line, 128, tracked_files_txt_ptr) != NULL)
	{
		fprintf(grep_files_txt_ptr, "%s", line);
	}
	fclose(tracked_files_txt_ptr);
	fclose(grep_files_txt_ptr);

	/* "objects" klasöründe "tree" ile başlayan dosyaların içeriklerini aktarma */
	/* son yapılan kaydın hash ini alma */
	fptr = fopen("./.kal/HEAD", "r");
	if(isNull(fptr, "HEAD could not be opened! (JustGrep)\n"))
		return;
	fscanf(fptr, "branch: %s", current_branch_path);
	fclose(fptr);

	fptr = fopen(current_branch_path, "r");
	if(isNull(fptr, "current branch file could not be opened! (JustGrep)\n"))
		return;
	fscanf(fptr, "%s", save_obj.hash);
	fclose(fptr);

	/* önceki kayıtlara giderek onların da tree dosyalarına erişmeyi sağlayan döngü */
	while(strcmp(save_obj.hash, "NONE") != 0 && GetFileSize(current_branch_path) != 0)
	{
		/* son yapılan kaydın hash ini kullanarak son tree ye erişme */
		GenerateFilePath(save_obj.file_path, save_obj.hash);

		/* save dosyasını açarak kayıda ait olan tree dosyasının hash ini ve bir önceki kaydın hash ini alma */
		fptr = fopen(save_obj.file_path, "r");
		if(isNull(fptr, "save file could not be opened! (JustGrep)\n"))
			return;
		fgets(line, 128, fptr);
		fgets(line, 128, fptr);
		sscanf(line, "tree %s\n", tree_obj.hash);
		fgets(line, 128, fptr);
		sscanf(line, "parent %s\n", save_obj.hash);
		fclose(fptr);

		GenerateFilePath(tree_obj.file_path, tree_obj.hash);

		/* son tree dosyasının içeriğini (ilk satır hariç) grep_files.txt dosyasına aktarma */
		grep_files_txt_ptr = fopen("./.kal/grep_files.txt", "a+");
		if(isNull(grep_files_txt_ptr, "grep_files.txt could not be opened! (JustGrep)\n"))
			return;
		fptr = fopen(tree_obj.file_path, "r");
		if(isNull(fptr, "tree file could not be opened! (JustGrep)\n"))
			return;
		fgets(line, 128, fptr);
		while(fgets(line, 128, fptr) != NULL)
		{
			fprintf(grep_files_txt_ptr, "%s", line);
		}
		fclose(fptr);
		fclose(grep_files_txt_ptr);
	}

	/* içeriği taranacak dosylar ve hash leri belirlendikten sonra dosyaların içeriklerinde tarama yapma */
	grep_files_txt_ptr = fopen("./.kal/grep_files.txt", "r");
	if(isNull(grep_files_txt_ptr, "grep_files.txt could not be opened! (JustGrep)\n"))
		return;
	while(fgets(line, 128, grep_files_txt_ptr) != NULL)
	{
		/* tracked_files.txt dosyasını satır satır okuma ve güncel olarak takip edilen dosyaların (klasörler de eklenmeli)
		isimlerini ve hash lerini alma */
		sscanf(line, "%s -> %s\n", file_name, file_hash);
		
		/* dosyaların hash lerini kullanarak sıkıştırılan içeriklerin dosya yolunu elde etme */
		GenerateFilePath(tracked_blob.file_path, file_hash);

		/* sıkıştırılan içeriklerin tutulduğu dosyanın yolunu kullanarak içeriği açarak geçici dosyaya aktarma */
		sprintf(command, "/bin/bash -c 'base64 -d %s | gunzip > ./.kal/temp_content_file.txt'", tracked_blob.file_path);
		system(command);

		/* açık içeriğin aktarıldığı geçici dosya açılarak eşleşme var mı kontrol etme */
		fptr = fopen("./.kal/temp_content_file.txt", "r");
		if(isNull(fptr, "temp_content_file.txt could not be opened! (JustGrep)\n"))
			return;
		line_num = 1;
		keyword_index = 0;
		offset = 0;
		match_count = 0;
		i = 0;
		while(c = getc(fptr))
		{
			offset++;
			if(c == '\n' || c == EOF)
			{
				if(match_count != 0)
				{
					fseek(fptr, -offset, SEEK_CUR);
					printf(MAG "%s" RESET, file_name); printf(CYN ":" RESET); printf(GRN "%d" RESET, line_num); printf(CYN ":" RESET);
					keyword_index = 0;
					i = 0;
					while(0 < match_count || (c != '\n' && c != EOF)) /* kontrol edilmeli */
					{
						c = getc(fptr);

						if(c == keyword[keyword_index])
						{
							keyword_index++;
						}
						else if(c == EOF)
						{
							strnprint(keyword, keyword_index);
						}
						else
						{
							strnprint(keyword, keyword_index);
							printf("%c", c);
							keyword_index = 0;
						}

						if(keyword_index == keyword_length)
						{
							match_count--;
							printf(RED "%s" RESET, keyword);
							keyword_index = 0;
						}
					}
					keyword_index = 0;
					if(c == EOF)
					{
						break;
					} 
				}
				else
				{
					keyword_index = 0;
				}
				line_num++;
				offset = 0;		
				if(c == EOF) break;
			}
			else if(c == keyword[keyword_index])
			{
				keyword_index++;
			}
			else
			{
				keyword_index = 0;
			}

			if(keyword_index == keyword_length)
			{
				match_count++;
			}
		}
		fclose(fptr);
	}
	fclose(grep_files_txt_ptr);

	/* silinecek dosyaların silinmesi */
	sprintf(command, "rm -f ./.kal/temp_content_file.txt");
	system(command);
	sprintf(command, "rm -f ./.kal/grep_files.txt");
	system(command);
}

void InsensitiveGrep(char* keyword)
{
	FILE* tracked_files_txt_ptr;
	FILE* grep_files_txt_ptr;
	FILE* fptr;
	char line[128];
	char file_name[32];
	char file_hash[41];
	blob_t tracked_blob;
	int line_num;
	int keyword_index;
	int keyword_length;
	int offset;
	int match_count;
	int i;
	char command[128];
	char c;
	save_t save_obj;
	tree_t tree_obj;
	char current_branch_path[64];

	keyword_length = strlen(keyword);

	/* içeriğinde arama yapılacak dosyaların ve hash lerini bir dosyaya aktarma */
	grep_files_txt_ptr = fopen("./.kal/grep_files.txt", "w");
	if(isNull(grep_files_txt_ptr, "grep_files.txt could not be generated! (JustGrep)\n"))
		return;
	/* tracked_files.txt dosyasının içeriğini kopyalama */
	tracked_files_txt_ptr = fopen("./.kal/tracked_files.txt", "r");
	if(isNull(tracked_files_txt_ptr, "tracked_files.txt could not be opened! (JustGrep)\n"))
		return;
	while(fgets(line, 128, tracked_files_txt_ptr) != NULL)
	{
		fprintf(grep_files_txt_ptr, "%s", line);
	}
	fclose(tracked_files_txt_ptr);
	fclose(grep_files_txt_ptr);

	/* "objects" klasöründe "tree" ile başlayan dosyaların içeriklerini aktarma */
	/* son yapılan kaydın hash ini alma */
	fptr = fopen("./.kal/HEAD", "r");
	if(isNull(fptr, "HEAD could not be opened! (JustGrep)\n"))
		return;
	fscanf(fptr, "branch: %s", current_branch_path);
	fclose(fptr);

	fptr = fopen(current_branch_path, "r");
	if(isNull(fptr, "current branch file could not be opened! (JustGrep)\n"))
		return;
	fscanf(fptr, "%s", save_obj.hash);
	fclose(fptr);

	/* önceki kayıtlara giderek onların da tree dosyalarına erişmeyi sağlayan döngü */
	while(strcmp(save_obj.hash, "NONE") != 0 && GetFileSize(current_branch_path) != 0)
	{
		/* son yapılan kaydın hash ini kullanarak son tree ye erişme */
		GenerateFilePath(save_obj.file_path, save_obj.hash);

		/* save dosyasını açarak kayıda ait olan tree dosyasının hash ini ve bir önceki kaydın hash ini alma */
		fptr = fopen(save_obj.file_path, "r");
		if(isNull(fptr, "save file could not be opened! (JustGrep)\n"))
			return;
		fgets(line, 128, fptr);
		fgets(line, 128, fptr);
		sscanf(line, "tree %s\n", tree_obj.hash);
		fgets(line, 128, fptr);
		sscanf(line, "parent %s\n", save_obj.hash);
		fclose(fptr);

		GenerateFilePath(tree_obj.file_path, tree_obj.hash);

		/* son tree dosyasının içeriğini (ilk satır hariç) grep_files.txt dosyasına aktarma */
		grep_files_txt_ptr = fopen("./.kal/grep_files.txt", "a+");
		if(isNull(grep_files_txt_ptr, "grep_files.txt could not be opened! (JustGrep)\n"))
			return;
		fptr = fopen(tree_obj.file_path, "r");
		if(isNull(fptr, "tree file could not be opened! (JustGrep)\n"))
			return;
		fgets(line, 128, fptr);
		while(fgets(line, 128, fptr) != NULL)
		{
			fprintf(grep_files_txt_ptr, "%s", line);
		}
		fclose(fptr);
		fclose(grep_files_txt_ptr);
	}

	grep_files_txt_ptr = fopen("./.kal/grep_files.txt", "r");
	if(isNull(grep_files_txt_ptr, "grep_files.txt could not be opened! (JustGrep)\n"))
		return;
	while(fgets(line, 128, grep_files_txt_ptr) != NULL)
	{
		/* tracked_files.txt dosyasını satır satır okuma ve güncel olarak takip edilen dosyaların (klasörler de eklenmeli)
		isimlerini ve hash lerini alma */
		sscanf(line, "%s -> %s\n", file_name, file_hash);
		
		/* dosyaların hash lerini kullanarak sıkıştırılan içeriklerin dosya yolunu elde etme */
		GenerateFilePath(tracked_blob.file_path, file_hash);

		/* sıkıştırılan içeriklerin tutulduğu dosyanın yolunu kullanarak içeriği açarak geçici dosyaya aktarma */
		sprintf(command, "/bin/bash -c 'base64 -d %s | gunzip > ./.kal/temp_content_file.txt'", tracked_blob.file_path);
		system(command);

		/* açık içeriğin aktarıldığı geçici dosya açılarak eşleşme var mı kontrol etme */
		fptr = fopen("./.kal/temp_content_file.txt", "r");
		if(isNull(fptr, "temp_content_file.txt could not be opened! (JustGrep)\n"))
			return;
		line_num = 1;
		keyword_index = 0;
		offset = 0;
		match_count = 0;
		i = 0;
		while(c = getc(fptr))
		{
			offset++;
			if(c == '\n' || c == EOF)
			{
				if(match_count != 0)
				{
					fseek(fptr, -offset, SEEK_CUR);
					printf(MAG "%s" RESET, file_name); printf(CYN ":" RESET);
					keyword_index = 0;
					i = 0;
					while(0 < match_count || (c != '\n' && c != EOF)) /* kontrol edilmeli */
					{
						c = getc(fptr);

						if(isInsensitive(c, keyword[keyword_index]))
						{
							keyword_index++;
						}
						else if(c == EOF)
						{
							strnprint(keyword, keyword_index);
						}
						else
						{
							strnprint(keyword, keyword_index);
							printf("%c", c);
							keyword_index = 0;
						}

						if(keyword_index == keyword_length)
						{
							match_count--;
							printf(RED "%s" RESET, keyword);
							keyword_index = 0;
						}
					}
					keyword_index = 0;
					if(c == EOF)
					{
						break;
					} 
				}
				else
				{
					keyword_index = 0;
				}
				line_num++;
				offset = 0;		
				if(c == EOF) break;
			}
			else if(isInsensitive(c, keyword[keyword_index]))
			{
				keyword_index++;
			}
			else
			{
				keyword_index = 0;
			}

			if(keyword_index == keyword_length)
			{
				match_count++;
			}
		}
		fclose(fptr);
	}
	fclose(grep_files_txt_ptr);

	/* silinecek dosyaların silinmesi */
	sprintf(command, "rm -f ./.kal/temp_content_file.txt");
	system(command);
	sprintf(command, "rm -f ./.kal/grep_files.txt");
	system(command);
}

int isInsensitive(char c1, char c2)
{
	int asci_diff = 'a' - 'A';
 
	/* dosyaların büyük küçük harfe dikkat etmeden aynı olup olmadığını kontrol eden fonksiyon */
	if(c1 == c2) return 1;
	else if(c2 > c1 && c2 - c1 == asci_diff) return 1;
	else if(c1 > c2 && c1 - c2 == asci_diff) return 1;
	else return 0;
}

void strnprint(char* str, int n)
{
	char* str2;

	str2 = (char*)malloc(sizeof(char) * strlen(str) + 1);
	strncpy(str2, str, n);
	str2[n] = '\0';
	printf("%s", str2);
	free(str2);
}

int CheckKalInit()
{
	if(!isInDir(".kal", "."))
	{
		printf("fatal: not a kal repository (or any of the parent directories): .kal\n");
		return 0;
	}
	else
		return 1;
}

int CheckForSave()
{
	struct passwd* pw;
	FILE* kalconfig_ptr;
	char config_file_path[64];
	char line[128];
	char keyword[16];
	int check = 0;

	/* ev dizinin yolunu alma */
	if((pw = getpwuid(getuid())) == NULL)
	{
		printf("getpwuid: no password entry\n");
		return 0;
	}

	/* kullanıcının bilgilerinin atanıp atanmadığını kontrol etme 
	eğer atanmadıysa kullanıcıyı bilgilendirme */
	strcpy(config_file_path, "./.kal/config");
	kalconfig_ptr = fopen(config_file_path, "r");
	if(kalconfig_ptr == NULL)
	{
		/* ayar dosyasının yolunun hazırlanması ve dosyanın açılmaya çalışılması */
		strcpy(config_file_path, pw->pw_dir);
		strcat(config_file_path, "/.kalconfig");
		kalconfig_ptr = fopen(config_file_path, "r");
	}
	if(isNull(kalconfig_ptr, "config file could not be opened! (Save)\n"))
	{
		printf("Config file does not exist!\n");
		PrintConfigWarning();
		return 0;
	}
	/* isim veya email bilgilerinin tam olup olmadığını kontrol etme */
	while(fgets(line, 128, kalconfig_ptr) != NULL)
	{
		if(strcmp(line, "[user]\n") == 0) continue;
		sscanf(line, "\t%s", keyword);
		if(strcmp(keyword, "name") == 0) check++;
		if(strcmp(keyword, "email") == 0) check++;
	}
	/* eğer isim veya email bilgileri tam değilse kullanıcı bilgilendirilmeli */
	if(check != 2)
	{
		PrintConfigWarning();
		if( !inConfig(config_file_path, "name") ) printf("Invalid user name!\n");
		if( !inConfig(config_file_path, "email") ) printf("Invalid user email!\n");
		return 0;
	}
	/* kalconfig_ptr kapatılabilir sanırım */
	fclose(kalconfig_ptr);
	return 1;
}

void CatFileType(char* short_hash)
{
	FILE* object_fptr;
	DIR* dir;
	DIR* object_dir;
	struct dirent* entry;
	struct dirent* object_entry;
	char object_path[57];
	char keyword[16];

	dir = opendir("./.kal/objects/");
	if(isNull(dir, "objects directory could not be opened! (CatFileType)\n"))
		return;
	while((entry = readdir(dir)) != NULL)
	{
		if((entry->d_name)[0] == '.')
			continue;

		/* girilen hash in ait olduğu klasörü bulma */
		if(short_hash[0] != (entry->d_name)[0] || short_hash[1] != (entry->d_name)[1])
			continue;

		/* objects klasöründeki her bir klasörün yolunu oluşturma ve içlerine girme */
		strcpy(object_path, "./.kal/objects/");
		strcat(object_path, entry->d_name);

		object_dir = opendir(object_path);
		if(isNull(object_dir, "the object directory could not be opened! (CatFileType)\n"))
			return;
		while((object_entry = readdir(object_dir)) != NULL)
		{
			if((object_entry->d_name)[0] == '.')
				continue;

			/* nesne klasöründeki dosyonın yolunu oluşturma ve okuma */
			strcat(object_path, "/");
			strcat(object_path, object_entry->d_name);

			object_fptr = fopen(object_path, "r");
			if(isNull(object_fptr, "the object file could not be opened! (CatFileType)\n"))
				return;
			fgets(keyword, 16, object_fptr);
			fclose(object_fptr);

			/* anahtar kelimenin değerine göre çıktı verme */
			if(strcmp(keyword, "save\n") == 0)
				printf("save\n");
			else if(strcmp(keyword, "tree\n") == 0)
				printf("tree\n");
			else
				printf("blob\n");
		}
		closedir(object_dir);
	}
	closedir(dir);
}

void CatFilePretty(char* short_hash)
{
	FILE* object_fptr;
	DIR* dir;
	DIR* object_dir;
	struct dirent* entry;
	struct dirent* object_entry;
	char object_path[57];
	char keyword[16];
	char command[128];
	char line[128];
	char file_name[32];
	char file_hash[41];

	dir = opendir("./.kal/objects/");
	if(isNull(dir, "objects directory could not be opened! (CatFileType)\n"))
		return;
	while((entry = readdir(dir)) != NULL)
	{
		if((entry->d_name)[0] == '.')
			continue;

		/* girilen hash in ait olduğu klasörü bulma */
		if(short_hash[0] != (entry->d_name)[0] || short_hash[1] != (entry->d_name)[1])
			continue;

		/* objects klasöründeki her bir klasörün yolunu oluşturma ve içlerine girme */
		strcpy(object_path, "./.kal/objects/");
		strcat(object_path, entry->d_name);
		object_dir = opendir(object_path);
		if(isNull(object_dir, "the object directory could not be opened! (CatFileType)\n"))
			return;
		while((object_entry = readdir(object_dir)) != NULL)
		{
			if((object_entry->d_name)[0] == '.')
				continue;

			if(strncmp(object_entry->d_name, &(short_hash[2]), 5) != 0)
				continue;

			/* kısa hash in ait olduğu dosyanın yolunu oluşturma ve okuma */
			strcat(object_path, "/");
			strcat(object_path, object_entry->d_name);

			/* obje dosyasını açıp dosyanın türünü öğrenmek için dosyanın ilk satırını okuma */
			object_fptr = fopen(object_path, "r");
			if(isNull(object_fptr, "the object file could not be opened! (CatFilePretty)\n"))
				return;
			fgets(keyword, 16, object_fptr);

			/* eğer nesne kayıt veya tree ise direkt dosyayı yazdır, 
			eğer blob ise dosyanın içeriğine gerekli çevirmeleri yaptıktan sonra yazdırma */
			if(strcmp(keyword, "save\n") == 0)
			{
				sprintf(command, "cat %s", object_path);
				system(command);
				printf("\n");
				fclose(object_fptr);
				break;
			}
			else if(strcmp(keyword, "tree\n") == 0)
			{
				while(fgets(line, 128, object_fptr) != NULL)
				{
					sscanf(line, "%s -> %s\n", file_name, file_hash);
					printf("blob %s %s\n", file_hash, file_name);
				}
				fclose(object_fptr);
				break;
			}
			else
			{
				sprintf(command, "/bin/bash -c 'base64 -d %s | gunzip > ./.kal/decoded_file.txt'", object_path);
				system(command);
				sprintf(command, "cat ./.kal/decoded_file.txt");
				system(command);
				sprintf(command, "rm ./.kal/decoded_file.txt");
				system(command);
				fclose(object_fptr);
				break;
			}
		}
		closedir(object_dir);
	}
	closedir(dir);
}

void FindCommonParentHash(char* branch1_hash, char* branch2_hash, char* common_parent_hash)
{
	/* 1. dal1 in hash ini kayıt dosyasını açma 
	   2. kayıt dosyasından bir önceki kaydın hash ini alarak döngüye girme
	   3. döngünün içinde dal2 nin hash i ile kayıt dosyasını açma
	   4. kayıt dosyasından bir önceki kaydın hash ini alarak döngüye girme
	   5. iki dalda da hash leri aynı olan kayda gelince o hash ortak atanın hash idir */
	   
	FILE* branch1_fptr;
	FILE* branch2_fptr;
	char branch1_file_path[64];
	char branch2_file_path[64];
	char branch1_parent_hash[41];
	char branch2_parent_hash[41];
	char line[128];

	GenerateFilePath(branch1_file_path, branch1_hash);
	GenerateFilePath(branch2_file_path, branch2_hash);

	strcpy(branch1_parent_hash, branch1_hash);
	while(strcmp(branch1_parent_hash, "NONE") != 0)
	{
		strcpy(branch2_parent_hash, branch2_hash);
		while(strcmp(branch2_parent_hash, "NONE") != 0)
		{
			if(strcmp(branch1_parent_hash, branch2_parent_hash) == 0)
			{
				strcpy(common_parent_hash, branch1_parent_hash);
				return;
			}
			branch2_fptr = fopen(branch2_file_path, "r");
			if(isNull(branch2_fptr, "branch 2 file path could not be opened! (FindCommonParentHash)\n"))
				return;
			fgets(line, 128, branch2_fptr);
			fgets(line, 128, branch2_fptr);
			fgets(line, 128, branch2_fptr);
			sscanf(line, "parent %s\n", branch2_parent_hash);
			fclose(branch2_fptr);
		}
		branch1_fptr = fopen(branch1_file_path, "r");
		if(isNull(branch1_fptr, "branch 1 file path could not be opened! (FindCommonParentHash)\n"))
			return;
		fgets(line, 128, branch1_fptr);
		fgets(line, 128, branch1_fptr);
		fgets(line, 128, branch1_fptr);
		sscanf(line, "parent %s\n", branch1_parent_hash);
		fclose(branch1_fptr);
	}
}


/* 3772 satır olmalı, 148 yeni satır (tamamlandı, merge1.c dosyasına geri kalanını yazdım) */