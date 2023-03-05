// Copyright Epic Games, Inc. All Rights Reserved.


#include "ProjGameModeBase.h"

#include "Proj.h"

/*void AProjGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	class GunBase
	{
	public:

		virtual void Shoot()
		{
			LOG("Shoot")
		}

		uint8 Data [500];

		TWeakPtr<GunBase> Sib = nullptr;
	};

	class Pistol : public GunBase
	{
	public:

		virtual void Shoot() override
		{
			LOG("Shoot pistol")
		}
	};

	class PhysGun : public GunBase
	{
	public:
		
		virtual void Shoot() override
		{
			LOG("Shoot physgun")
		}
	};

	TSharedPtr<GunBase> Gun1 = MakeShared<GunBase>();
	TSharedPtr<GunBase> Gun2 = MakeShared<GunBase>();

	Gun1->Sib = Gun2;
	Gun2->Sib = Gun1;
	
	TArray<TSharedPtr<GunBase>> Guns;
	
	for (int i = 0; i < 10; ++i)
	{
		TSharedPtr<PhysGun> G = MakeShared<PhysGun>();
		G->Shoot();
		
		Guns.Add(G);
		static_cast<GunBase*>(Guns[i].Get())->Shoot();
		Guns[i]->Shoot();
		Guns[i].Get()->Shoot();
		
		G->Shoot();
	}
}*/

namespace Tests
{
	// Forward declaration
	class FFolderItem;
	class FFolder;
	class FHierarchyItem;

	TSharedPtr<FFolder> RootFolder;
	TSharedPtr<FHierarchyItem> OpenedFolder;
	TSharedPtr<FHierarchyItem> OpenedItem;

	/*
	 * The base class for everything related to folder-item logic
	 */
	class FHierarchyItem : public TSharedFromThis<FHierarchyItem> // Allows to use the 'SharedThis' function to convert the 'this' pointer to 'TSharedPtr'
	{
	public:

		// Ignore this for now, unreal does this by default
		virtual ~FHierarchyItem() = default;

		TSharedPtr<FHierarchyItem> GetParent() { return Parent; }
		int32 GetIndex() { return Index; }
	
	public: // Interface for children to override. Will throw if called without being overridden
	
		virtual FString GetName() { check(false) return ""; }
		virtual bool IsFolder() { check(false) return false; }

		// A custom logic for each item when they are opened.
		virtual void Open()
		{
			if (IsFolder())
			{
				OpenedFolder = SharedThis(this);
			}
		
			OpenedItem = SharedThis(this);
		}
	
	protected: // Sorting logic

		// Position inside it's parent
		int32 Index = 0;

		// Name of this Item
		FString Name = "";

	private:

		// The folder this item is inside of. Null if root.
		TSharedPtr<FHierarchyItem> Parent = nullptr;

	public: // Setters

		void SetName(const FString& NewName) { Name = NewName; }

	};
	
	/*
	 * The folder class stores all kinds of items inside of it.
	 */
	class FFolder : public FHierarchyItem
	{
		private: // Ignore. This is done by GENERATED_BODY()

		using Super = FHierarchyItem;
	
	public:
	
		virtual FString GetName() override { return FString::Printf(TEXT("(Folder) - %s"), *Name); }
		virtual bool IsFolder() { return true; }

		virtual void Open() override
		{
			Super::Open();

			UE_LOG(LogTemp, Warning, TEXT("Opened a folder named: '%s'"), *GetName());
		}

	private: // Folder logic

		TArray<TSharedPtr<FHierarchyItem>> Children = {};

	public: // Setters

		void AddChild(TSharedPtr<FHierarchyItem> NewChild)
		{
			Children.Add(NewChild);
		}

	public:

		TArray<TSharedPtr<FHierarchyItem>> GetChildren() { return Children; }
	};

	/*
	 * The base class for all items that can be stored inside of folders.
	 */
	class FFolderItem : public FHierarchyItem
	{
	public:
	
		virtual bool IsFolder() { return false; }
	
	};

	/*
	 * A texture that can be rendered in the form a a byte array. 1 byte = 1 pixel
	 */
	struct FFolderTexture
	{
		uint32 Width, Height;
		TArray<uint8> TextureData;
	};

	/*
	 * This item can store a texture like a .png, .jpg or any other image types.
	 */
	class FFolderTextureItem : public FFolderItem
	{
	private: // Ignore. This is done by GENERATED_BODY()

		using Super = FHierarchyItem;
	
	private: // Data
	
		TUniquePtr<FFolderTexture> Texture;
	
	public: // FFolderITem Interface
	
		virtual FString GetName() override { return FString::Printf(TEXT("(Texture) - %s"), *Name); }
		virtual void Open() override
		{
			Super::Open();

			UE_LOG(LogTemp, Warning, TEXT("Opened a texture item: '%s'. The texture has (%i) bytes of data!"), *GetName(), Texture->TextureData.Num())
		}
	};

	/*
	 * A .txt file for one line
	 * Please use this if you actually only use 1 line. It's really faster!!!!!
	 * Can ONLY store one line of text. Use FFolderMultiTextItem if you require more.
	 */
	class FFolderTextItem : public FFolderItem
	{
	private: // Ignore. This is done by GENERATED_BODY()

		using Super = FHierarchyItem;
	
	private: // Data

		// DONT EVER using a \n character!!! It WILL break this!
		FString Text;
	
		public: // FFolderITem Interface
	
		virtual FString GetName() override { return FString::Printf(TEXT("(Text) - %s"), *Name); }
		virtual void Open() override
		{
			Super::Open();

			UE_LOG(LogTemp, Warning, TEXT("Opened a text item: '%s'. The texture contains a (%s) string!"), *GetName(), *Text)
		}

	public: // Setters

		void SetText(const FString& InText)
		{
			if (GIsEditor) // Editor safety check
				{
				for (TCHAR character : InText)
				{
					// Make sure nobody is doing anything fUnkY
					check(character != TEXT('\n'))
				}
				}

			Text = InText;
		}

	public: // Getters

		FString GetText() { return Text; }
	};

	/*
	 * A .txt file for MULTIPLE lines
	 * Please use this ONLY if you actually need more than one line. Consider using a few FFolderTextItems internally if you only need a few lines. It WILL still be faster!!!
	 */
	class FFolderMultiTextItem : public FFolderItem
	{
		private: // Ignore. This is done by GENERATED_BODY()

		using Super = FHierarchyItem;
	
		private: // Data
	
		TArray<FString> Text;
	
		public: // FFolderITem Interface
	
		virtual FString GetName() override { return FString::Printf(TEXT("(Text) - %s"), *Name); }
		virtual void Open() override
		{
			Super::Open();

			FString AllText = "";

			for (const FString& String : Text)
			{
				AllText += String;
			}
		
			UE_LOG(LogTemp, Warning, TEXT("Opened a text item: '%s'. The texture contains a (%s) string!"), *GetName(), *AllText)
		}

		public: // Setters

		void SetText(const TArray<FString>& InText)
		{
			if (GIsEditor) // Editor safety check
				{
				for (const FString& Line : InText)
				{
					for (TCHAR character : Line)
					{
						// Make sure nobody is doing anything fUnkY
						check(character != TEXT('\n'))
					}
				}
				}

			Text = InText;
		}
	
		TArray<FString> GetText() { return Text; }
	};

	TSharedPtr<FFolder> MakeFolder(const FString& Name)
	{
		TSharedPtr<FFolder> Folder = MakeShared<FFolder>();
		
		Folder->SetName(Name);
		
		return Folder;
	}
}

// 1 - Create a root folder 'C:' and add Folder '#1 - #5' to the door.
// 2 - Then add a folder called 'MyFolder' every folder that was created in stage 1
// 3 - Then create random items and put them into random folders
// 4 - The number of folders created in stage 1 are decided by a variable
// 5 - The number of items created in stage 3 is decided by a variable
// 6 - Fix all TODOs
// 7 - Refactor (this is terrible, just change everything. DONT USE NAMESPACES IN OUR PROJECT, If there are naming conflicts, just rename your class. Our coding standards explicitly FORBID using namespaces)
// And don't forget that namespaces aren't the only thing wrong with this!

void AProjGameModeBase::BeginPlay()
{
	return;
	
	Tests::RootFolder = Tests::MakeFolder("C:");

	Tests::RootFolder->AddChild(Tests::MakeFolder("Folder #1"));
	Tests::RootFolder->AddChild(Tests::MakeFolder("Folder #2"));
	Tests::RootFolder->AddChild(Tests::MakeFolder("Folder #3"));
	Tests::RootFolder->AddChild(Tests::MakeFolder("Folder #4"));
	Tests::RootFolder->AddChild(Tests::MakeFolder("Folder #5"));

	// Add a new folder named MyFolder to all folders
	TArray<Tests::FFolder*> AllFoundFolders;
	TArray<TSharedPtr<Tests::FHierarchyItem>> RemainingItems { Tests::RootFolder };
	TArray<TSharedPtr<Tests::FHierarchyItem>> CheckedItems;
	while (RemainingItems.Num() > 0)
	{
		// Get and remove first remaining item
		TSharedPtr<Tests::FHierarchyItem> CurrentItem = RemainingItems[0];
		RemainingItems.RemoveAt(0);

		// Don't check this ptr again
		CheckedItems.Add(CurrentItem);

		if (CurrentItem == nullptr || !CurrentItem->IsFolder())
		{
			continue;
		}

		// Unwrap current item pointer so we can more easily cast it
		Tests::FHierarchyItem* CurrentItemRaw = CurrentItem.Get();
		Tests::FFolder* CurrentFolderRaw = static_cast<Tests::FFolder*>(CurrentItemRaw);

		AllFoundFolders.Add(CurrentFolderRaw);
		
		for (TSharedPtr<Tests::FHierarchyItem> ChildItem : CurrentFolderRaw->GetChildren())
		{
			// Add all children to remaining items if we haven't checked them yet
			if (!CheckedItems.Contains(ChildItem))
			{
				RemainingItems.Add(ChildItem);
			}
		}
	}
	for (Tests::FFolder* folder : AllFoundFolders)
	{
		folder->AddChild(Tests::MakeFolder("MyFolder"));
	}
	// Now randomly add a few items to the original folders
	int32 ItemCount = FMath::RandRange(8, 27);
	for (int32 i = 0; i < ItemCount; ++i)
	{
		// Get a random folder
		Tests::FFolder* folder = AllFoundFolders[FMath::RandRange(0, AllFoundFolders.Num() - 1)];

		TSharedPtr<Tests::FHierarchyItem> CreatedItem;

		// TODO Better item names
		switch (FMath::RandRange(0, 2))
		{
		case 0:
			CreatedItem = MakeShared<Tests::FFolderTextureItem>();
			CreatedItem->SetName("Texture");
			// TODO Textures won't have data right now!
			break;

		case 1:
		{
			TSharedPtr<Tests::FFolderTextItem> text = MakeShared<Tests::FFolderTextItem>();
			text->SetName("Text");
			text->SetText("Some text"); // TODO Remove placeholder
			
			CreatedItem = text;
		} break;
			
		case 2:
		{
			TSharedPtr<Tests::FFolderMultiTextItem> text = MakeShared<Tests::FFolderMultiTextItem>();
			text->SetName("Text");
			text->SetText({"Line 1", "Line 2", "Line\n3"}); // TODO Who thought this was a good idea???
			
			CreatedItem = text;
				
		} break;

		default: check(false); // Someone fucked up
		}

		folder->AddChild(CreatedItem);
	}

	// print EVERYTHING ^-^
	TArray<TSharedPtr<Tests::FHierarchyItem>> RemainingItemsToPrint { Tests::RootFolder };
	TArray<TSharedPtr<Tests::FHierarchyItem>> CheckedItemsToPrint;
	while (RemainingItemsToPrint.Num() > 0)
	{
		// Get and remove first remaining item
		TSharedPtr<Tests::FHierarchyItem> CurrentItem = RemainingItemsToPrint[0];
		RemainingItemsToPrint.RemoveAt(0);

		// Don't check this ptr again
		CheckedItemsToPrint.Add(CurrentItem);

		if (CurrentItem == nullptr)
		{
			continue;
		}

		// TODO Logging everything individually is slow, cache the strings if performance becomes an issue
		UE_LOG(LogTemp, Log, TEXT("%s"), *CurrentItem->GetName());
		
		if (CurrentItem->IsFolder())
		{
			Tests::FHierarchyItem* CurrentItemRaw = CurrentItem.Get();
			Tests::FFolder* CurrentFolderRaw = static_cast<Tests::FFolder*>(CurrentItemRaw);
			
			for (TSharedPtr<Tests::FHierarchyItem> ChildItem : CurrentFolderRaw->GetChildren())
			{
				check(!CheckedItemsToPrint.Contains(ChildItem)); // There should NEVER be a duplicate! This is a folder tree!
				
				RemainingItemsToPrint.Add(ChildItem);
			}
		}
	}
}