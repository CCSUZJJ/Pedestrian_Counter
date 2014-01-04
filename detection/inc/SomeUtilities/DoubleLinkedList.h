////////////////////////////////////////////////////////////////////////////
//
//	Name		:	DoubleLinkedList.h
//
//	Description	: Double Linked List Template.
//
//                This template implements a lowlevel-doublelinked list where
//                fast operations on the listhandling is the goal. For this
//                reason the caller has access to the container-class, such
//                that caller can move the containers freely into the list or
//                to other lists (In order to prevent subsequent delete/new-operations)
//
//                As speed is the goal of this implementation, no extensive checking
//                on the list is done (eg. if a reference is assumed to be in the
//                list, then it is not checked if its really is in the list).
//
//                Because the number of entries in the list is often requested,
//                the number of containers is counted. As the caller has free access
//                to the container, one can remove the contents from the container
//                and leave the empty container into the list. In this particular
//                case the number of entries will not be equal to the number of containers.
//
//                In order to have threadsafety (if needed), one has to implement a mutex
//                Scheme : Access Mutex - do list operations - Release Mutex. The locking
//                of the list is not handled by this template and is not its intention.
//
//                Common errors :
//                    As soon as the object reaches the scope, the destructor will be called.
//                    This means that all containers and their contents will be deleted. If one
//                    is still referencing an entry the reference will become invalid.
//
//                    Extract container or data and not delete them afterwards

#ifndef DOUBLELINKEDLIST_HH_
#define DOUBLELINKEDLIST_HH_
//
//   DoubleLinkedList-parameters
//         - Head
//         - Tail
//         - EntriesInList
//
//
//   Conventions :
//
//         [1] Empty-list (any parameter can be checked)
//							- Head = 0
//							- Tail = 0
//							- EntriesInList = 0
//
//         [2] Re-entrancy (thread-safety) must be handled by the
//             user (is not part of this structure)
//
//         [3] At destruction, the destructor for all containers
//             will be called ... (If this effect is not wanted, then
//             the user will have to remove the containers from the list)
//
//

template<class T> class DoubleLinkedList
{
public :

// Container-class

template<class T1> class Container
	{
	private :

	Container(const Container&);				// prevent copy_constructor

	public :

	// For those who prefer getters

	inline T1* GetContents() {return Data;}
	inline const T1* GetContents() const {return Data;}

	// 3 pointers

	Container<T1>*	ToHead;						// pointer to container in the head-direction
	Container<T1>*	ToTail;						// pointer to container in the tail-direction
	T1*				Data;						// pointer to payload (can be null if payload is not available anylonger)

	// Constructors

	Container(T1* Data=0):Data(Data) {}

	// Destructor
	//   The convention is that if the Data != 0, the data pointed to
	//   is also destructed. The user can prevent this of course by setting
	//   the Data = 0. (Should then take care of the data !!!)

	~Container()
		{
		if (Data != 0) delete Data;
		Data=0;				// defensive programming !!
		}
	};

private:
	Container<T>*		Tail;
	Container<T>*		Head;
    unsigned int		EntriesInList;			// = number of containers

	DoubleLinkedList(const DoubleLinkedList&);	// prevent copy_constructor

public:

	// Constructor

	DoubleLinkedList()
		{
		EntriesInList=0;
		Tail=0;
		Head=0;
		}

	// Destructor (See note on Container)

	~DoubleLinkedList()
		{
		Reset();
		}

	// Get a pointer to the last container in the list -> Tail
	// in case of an empty list, 0 is returned

	inline Container<T>* GetContainerAtTail() const
		{
		return Tail;
		}

	// Get a pointer to the first container in the list -> Head
	// in case of an empty list, 0 is returned

	inline Container<T>* GetContainerAtHead() const
		{
		return Head;
		}

	// Get the number of entries == containers in the list

    inline unsigned int GetNumberOfEntries() const
		{
		return EntriesInList;
		}

	// Get a pointer to the last data in the list -> Tail
	//  --> Data remains in the list !
	// in case of an empty list or container, 0 is returned

	inline T* GetDataAtTail() const
		{
		if (Tail == 0) return 0;
		return Tail->Data;
		}

	// Get a pointer to the first data in the list -> Head
	//  --> Data remains in the list !
	// in case of an empty list or container, 0 is returned

	inline T* GetDataAtHead() const
		{
		if (Head == 0) return 0;
		return Head->Data;
		}

	// +-------+
	// | Reset |
	// +-------+
	//
	//  Description : Removes (deletes) all entries from the list. As the
	//    entries are deleted, the data pointed to (if any) are also deleted
	//    (See remark on Container-destructor)

	void Reset()
		{
		Container<T>*	Temp;

		// List is empty ?

		if(Tail == 0) return;

		// The list contains at least 1 container

		while ((Temp = Tail->ToHead) != 0)
				{
				delete Tail;
				Tail = Temp;
				}

		// Delete the last entry

		delete Tail;

		Tail = 0;
		Head = 0;
		EntriesInList = 0;
		}


	// +----------------------------------+
	// | Add data at the head of the list |
	// +----------------------------------+
	//
	// Description : Creates a container and adds the container
	//               at the head of the list

	void AddDataAtHead(T* Data)
		{
		AddContainerAtHead(new Container<T>(Data));
		}

	// +----------------------------------+
	// | Add data at the tail of the list |
	// +----------------------------------+
	//
	// Description : Creates a container and adds the container
	//               at the tail of the list


	void AddDataAtTail(T* Data)
		{
		AddContainerAtTail(new Container<T>(Data));
		}

	// +-------------+
	// | Delete head |
	// +-------------+
	//
	//  Description : Deletes the container and the container-contents,
	//                that is at the head of the list. In case there is
	//                no head nothing is done.
	//

	void DeleteHead()
		{
		// List is empty ?

		if(Head == 0) return;

		// The list contains at least 1 container

		Container<T>* Temp = Head->ToTail;
		delete Head;
		Head = Temp;	// new head

		// An container at the head of the list has ToHead = 0
		// Still containers in list ?

		if (Head != 0) Head->ToHead = 0;
		else Tail = 0;

		// Update EntriesInList accordingly

		EntriesInList--;
		}

	// +-------------+
	// | Delete tail |
	// +-------------+
	//
	//  Description : Deletes the container and the container-contents,
	//                that is at the tail of the list. In case there is
	//                no tail nothing is done.
	//

	void DeleteTail()
		{
		// List is empty ?

		if (Tail == 0) return;

		// The list contains at least 1 container

		Container<T>* Temp = Tail->ToHead;
		delete Tail;
		Tail = Temp;		// new tail

		// An container at the tail of the list has ToTail = 0
		// Still containers in list ?

		if (Tail != 0) Tail->ToTail = 0;
		else Head = 0;

		// Update EntriesInList accordingly

		EntriesInList--;
		}


	// +--------------------------------------+
	// | Extract data at the head of the list |
	// +--------------------------------------+
	//
	// Description : In case the head exists, return the data at the head (else return 0)
	//               and delete the according container

	T* ExtractDataAtHead()
		{
		if (Head == 0) return 0;				// Empty list

		// Get the container

		T* DataOI = Head->Data;
		Head->Data = 0;							// prevend destruction upon deletion of the entry !
		DeleteHead();

		// Pass to caller

		return DataOI;
		}

	// +--------------------------------------+
	// | Extract data at the tail of the list |
	// +--------------------------------------+
	//
	// Description : In case the tail exists, return the data at the tail (else return 0)
	//               and delete the according container

	T* ExtractDataAtTail()
		{
		if (Tail == 0) return 0;				// Empty list

		// Get the container

		T* DataOI = Tail->Data;
		Tail->Data = 0;							// prevend destruction upon deletion of the entry !
		DeleteTail();

		// Pass to caller

		return DataOI;
		}


	// +------------------------------------------------------------+
	// | Insert container (direction = head) "before" another entry |
	// +------------------------------------------------------------+
	//
	// Description : Inserts the container. Insert is to be seen as adding
	//               the entry between the referenced entry and the head of the list
	//
	// Remarks : It is assumed that the Reference belongs to the list and that
	//           the NewEntry does not belong to any list (has been extracted somewhere)
	//
	//           In case the Reference == 0, then the entry is put at the head of the list

	void InsertToHead(Container<T>* Reference,Container<T>* NewEntry)
		{
		// Insert before head ?

		if ((Reference == Head) || (Reference == Head)) AddContainerAtHead(NewEntry);
		else
			{
			// Insert entry after the head somewhere
			//  Adjust next and previous pointer of the container to insert

			NewEntry->ToHead = Reference->ToHead;
			NewEntry->ToTail = Reference;

			// Adjust linkages in the list

			Reference->ToHead->ToTail = NewEntry;
			Reference->ToHead = NewEntry;

			// Update EntriesInList accordingly

			EntriesInList++;
			}
		}

	// +-----------------------------------------------------------+
	// | Insert container (direction = tail) "after" another entry |
	// +-----------------------------------------------------------+
	//
	// Description : Appends the container. Append is to be seen as adding
	//               the entry between the referenced entry and the tail of the list
	//
	// Remarks : It is assumed that the Reference belongs to the list and that
	//           the NewEntry does not belong to any list
	//
	//           In case the Reference == 0, then the entry is put at the tail of the list

	void InsertToTail(Container<T>* Reference,Container<T>* NewEntry)
		{
		// Append after head ?

		if ((Reference == Tail) || (Reference == Tail)) AddContainerAtTail(NewEntry);
			else
				{
				// Append entry before the head
				//  Adjust next and previous pointer of the container to insert

				NewEntry->ToHead = Reference;
				NewEntry->ToTail = Reference->ToTail;

				// Adjust linkages in the list

				Reference->ToTail->ToHead = NewEntry;
				Reference->ToTail = NewEntry;

				// Update EntriesInList accordingly

				EntriesInList++;
				}
		}

	// +-------------------------------------------------------+
	// | Insert data (direction = head) "before" another entry |
	// +-------------------------------------------------------+
	//
	// Description : Creates a container and inserts the container
	//               Insert is to be seen as adding the entry between
	//               the referenced entry and the head of the list
	//
	// Remarks : It is assumed that the ReferencedEntry belongs to the list.
	//           In case the Reference == 0, then the entry is put at the head of the list

	void InsertDataToHead(Container<T>* Reference,T* Data)
		{
		InsertToHead(Reference,new Container<T>(Data));
		}

	// +------------------------------------------------------+
	// | Insert data (direction = tail) "after" another entry |
	// +------------------------------------------------------+
	//
	// Description : Creates a container and appends the container
	//             Append is to be seen as adding the entry between
	//             the referenced entry and the tail of the list
	//
	// Remarks : It is assumed that the ReferencedEntry belongs to the list.
	//           In case the Reference == 0, then the entry is put at the tail of the list

	void InsertDataToTail(Container<T>* Reference,T* Data)
		{
		InsertToTail(Reference,new Container<T>(Data));
		}


	// +--------------------+
	// | Delete a container |
	// +--------------------+
	//
	// Description : A container is deleted (also the destructor of the container pointed to
	//
	// If one wants to keep the data, then the data must be extracted (Set Data to zero)
	//
	// Remarks : It is assumed that the container belongs to the list !

	void DeleteContainer(Container<T>* EntryToDelete)
		{
		// List is empty ?

		if (EntryToDelete == Head)
			{
			DeleteHead();
			return;
			}

		if (EntryToDelete == Tail)
			{
			DeleteTail();
			return;
			}

	// Item somewhere in between
	// Adjust linkages (Item is not at head nor at tail !!)

	EntryToDelete->ToHead->ToTail = EntryToDelete->ToTail;
	EntryToDelete->ToTail->ToHead = EntryToDelete->ToHead;

	delete EntryToDelete;

	// Update EntriesInList accordingly

	EntriesInList--;
	}

	// +--------------------------------+
	// | Delete list_entries until head |
	// +--------------------------------+
	//
	// Description : Delete all list-entries from this container until the head of
	//               the list. One can specify to delete the pointed entry to or not.
	//
	//
	// Remarks : It is assumed that the container belongs to the list !

	void DeleteUntilHead(Container<T>* Reference,bool DeleteReference)
		{
		while (Head != Reference) DeleteHead();

		// Delete the Reference too ? (which must now be the head entry)

		if (DeleteReference) DeleteHead();
		}

	// +--------------------------------+
	// | Delete list_entries until tail |
	// +--------------------------------+
	//
	// Description : Delete all list-entries from this container until the tail of
	//               the list. One can specify to delete the pointed entry to or not.
	//
	//
	// Remarks : It is assumed that the container belongs to the list !

	void DeleteUntilTail(Container<T>* Reference,bool DeleteReference)
		{
		while (Tail != Reference) DeleteTail();

		// Delete the Reference too ?

		if (DeleteReference) DeleteTail();
		}


	// +---------------------+
	// | Extract a container |
	// +---------------------+

	// Description : The container is extracted from the list and under complete
	//     control of the caller (The intention is to add this container into
	//     another list or to move it into a(nother) list)
	//
	// Remarks : It is assumed that the container belongs to the list !
	//           (So reference must != 0)

	Container<T>* ExtractContainer(Container<T>* EntryToExtract)
		{
		// Extract head ?

		if (EntryToExtract == Head)
			{
			Head = Head->ToTail;

			// An container at the head of the list has ToHead = 0
			// Still containers in list ?

			if (Head != 0) Head->ToHead = 0;
			else Tail = 0;
			}

		// Extract tail ?

		else if (EntryToExtract == Tail)
				{
				Tail = Tail->ToHead;

				// An container at the tail of the list has ToTail = 0
				// Still containers in list ?

				if (Tail != 0) Tail->ToTail = 0;
				else Head = 0;
				}

		// Entry somewhere in between

		else
			{
			// Adjust linkages (Item is not at head or at tail !!)

			EntryToExtract->ToHead->ToTail = EntryToExtract->ToTail;
			EntryToExtract->ToTail->ToHead = EntryToExtract->ToHead;
			}

		// Update EntriesInList accordingly

		EntriesInList--;

		// Defensive programming

		EntryToExtract->ToHead = 0;
		EntryToExtract->ToTail = 0;

		// Return the extracted entry

		return EntryToExtract;
		}

	// +--------------+
	// | Extract head |
	// +--------------+

	// Description : If the head exists, it is extracted from the list and under complete
	//     control of the caller (The intention is to add this container into
	//     another list or to move it into a(nother) list)
	//

	Container<T>* ExtractHead()
		{
		// Head exists ?

		if (Head == 0) return 0;					// List is empty
		return ExtractContainer(Head);
		}

	// +--------------+
	// | Extract tail |
	// +--------------+

	// Description : If the tail exists, it is extracted from the list and under complete
	//     control of the caller (The intention is to add this container into
	//     another list or to move it into a(nother) list)
	//

	Container<T>* ExtractTail()
		{
		// Tail exists ?

		if (Tail == 0) return 0;					// List is empty
		return ExtractContainer(Tail);
		}

	// +--------------+
	// | Extract data |
	// +--------------+

	// Description : The data is extracted from the list. The container is deleted.
	//
	// Remarks : It is assumed that the container belongs to the list !
	//

	T* ExtractData(Container<T>* EntryToExtract)
		{
		Container<T>* EntryOI = ExtractContainer(EntryToExtract);

		// Get the container

		T* ExtractedData = EntryOI->Data;
		EntryOI->Data = 0;							// prevend destruction upon deletion of the entry !
		delete EntryOI;

		// Pass to caller

		return ExtractedData;
		}

	// +-------------------+
	// | Extract container |
	// +-------------------+

	// Description : The container is extracted from the list. The container
	//   is under control of the caller
	//
	// Timing : List must be searched through !

	void ExtractContainer(T* ItemToExtract)
		{
		// Search the containerOI

		Container<T>* EntryOI = Head;
		while (EntryOI != 0)
				{
				if (EntryOI->Data == ItemToExtract)
					{
					// Got you !

					ExtractContainer(EntryOI);
					return;
					}

				EntryOI = EntryOI->ToTail;
				}
			}


	// +---------------------------------------+
	// | Add container at the head of the list |
	// +---------------------------------------+
	//
	// Description : Add the container at the head of the list. It is
	//   assumed that the container is checked by the caller, such that
	//   entry is pointing to a proper container !
	//
	//  Remark : It is assumed that the container does not belong to any list
	//

	void AddContainerAtHead(Container<T>* NewEntry)
		{
		// An container at the head of the list has ToHead = 0

		NewEntry->ToHead = 0;

		// Empty list ?

		if (Head == 0)
			{
			// List is empty

			NewEntry->ToTail = 0;			// Is also last container in list
			Tail = NewEntry;
			}
		else
			{
			// List is not empty

			NewEntry->ToTail = Head;
			Head->ToHead = NewEntry;
			}

		// New container at the head of the list

		Head = NewEntry;

		// Update EntriesInList accordingly

		EntriesInList++;
		}


	// +--------------------------------------+
	// | Add an entry at the tail of the list |
	// +--------------------------------------+
	//
	// Description : Add the given entry at the tail of the list. It is
	//   assumed that the entry is checked by the caller, such that
	//   Data is pointing to a proper container !
	//
	//  Remark : It is assumed that the entry does not belong to any double_linked_list
	//

	void AddContainerAtTail(Container<T>* NewEntry)
		{
        // A container at the tail of the list has ToTail = 0

		NewEntry->ToTail = 0;

		// Empty list ?

		if (Tail == 0)
			{
			// List is empty

			NewEntry->ToHead = 0;			// Is also first container in list
			Head = NewEntry;
			}
		else
			{
			// List is not empty

			NewEntry->ToHead = Tail;
			Tail->ToTail = NewEntry;
			}

		// New entry at the tail of the list

		Tail = NewEntry;

		// Update EntriesInList accordingly

		EntriesInList++;
		}

	// +------------------------+
	// | Extract all containers |
	// +------------------------+
	//
	// Description : Extracts all given containers. After this operation the given
	//   list will be empty. The number of entries is returned.
	//

    unsigned int ExtractAllContainers(Container<T>** NewHead,Container<T>** NewTail)
				{
                unsigned int EntriesExtracted = EntriesInList;

				*NewHead = Head;
				*NewTail = Tail;

				// Transferred ==> empty this list

				Head = 0;
				Tail = 0;
				EntriesInList = 0;

				// Exit

				return EntriesExtracted;
				}

	// +--------------------------------------------+
	// | Add the given list at the tail of the list |
	// +--------------------------------------------+
	//
	// Description : Extracts all containers from the given list and adds
	//   them at the tail of my list. After this operation the given
	//   list will be empty.
	//

	void AddListAtTail(DoubleLinkedList<T>& ListToAdd)
		{
		// Transfer list

		Container<T>* HeadToAdd;
		Container<T>* TailToAdd;

		EntriesInList += ListToAdd.ExtractAllContainers(&HeadToAdd,&TailToAdd);
		if (HeadToAdd == 0) return;					// Empty list ...

		// My list is empty ?

		if (Tail == 0)
			{
			// List is empty

			Head = HeadToAdd;
			}
			else
				{
				// My list is not empty

				HeadToAdd->ToHead = Tail;
				Tail->ToTail = HeadToAdd;
				}

		// New entry at the tail of the list

		Tail = TailToAdd;
		}

	// +--------------------------------------------+
	// | Add the given list at the head of the list |
	// +--------------------------------------------+
	//
	// Description : Extracts all containers from the given list and inserts
	//   them at the head of my list. After this operation the given
	//   list will be empty.

	void AddListAtHead(DoubleLinkedList<T>& ListToAdd)
		{
		// Transfer list

		Container<T>* HeadToAdd;
		Container<T>* TailToAdd;

		EntriesInList += ListToAdd.ExtractAllContainers(&HeadToAdd,&TailToAdd);
		if (HeadToAdd == 0) return;					// Empty list ...

		// My list is empty ?

		if (Head == 0)
			{
			// List is empty

			Tail = TailToAdd;
			}
			else
				{
				// My list is not empty

				TailToAdd->ToTail = Head;
				Head->ToHead = TailToAdd;
				}

		// New entry at the head of the list

		Head = HeadToAdd;
		}
};

#endif /*DOUBLELINKEDLIST_HH_*/
