////////////////////////////////////////////////////////////////////////////////////////////////////////////
// библиоотека patcher_x86.dll 
// распространяется свободно(бесплатно)
// авторское право: Баринов Александр (baratorch), e-mail: baratorch@yandex.ru
// форма реализации низкоуровневых хуков (LoHook) отчасти позаимствована у Berserker (из ERA)
////////////////////////////////////////////////////////////////////////////////////////////////////////////

// версия 4.2

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ОПИСАНИЕ.
//
// ! библиотека предоставляет:
//  - удобные унифицированные централизованные 
//    инструменты для установки патчей и хуков
//    в код целевой программы.
//  - дополнительные инструменты: дизассемблер длин опкодов и функцию
//    копирующую код с корректным переносом опкодов jmp и call c 
//    относительной адресацией
// ! библиотека позволяет
//  - устанавливать как простые так и сложные патчи.
//  - устанавливать высокоуровневые хуки, замещая оригинальные функции в
//    в целевом коде на свои, не заботясь о регистрах процессора,
//    стэке, и возврате в оригинальный код.
//  - устанавливать высокоуровневые хуки один на другой
//    не исключая а дополняя при этом функциональность хуков
//    установленных раньше последнего
//  - устанавливать низкоуровневые хуки с высокоуровневым доступом к
//    регистрам процессора, стэку, затертому коду и адресу возврата в код
//  - отменять любой патч и хук, установленный с помощью этой библиотеки.
//  - узнать задействован ли определенный мод, использующий библиотеку
//  - узнать какой мод (использующий библиотеку) установил определенный патч/хук
//  - получить полный доступ ко всем патчам/хукам, установленным из других модов 
//    с помощью этой библиотеки
//  - легко и быстро обнаружить конфликтующие патчи из разных модов
//    (использующих эту библиотеку) 1) отмечаяв логе такие конфликты как:
//        - устанавливаются патчи/хуки разного размера на один адрес
//        - устанавливаются патчи/хуки перекрывающие один другого со смещением
//        - устанавливаются патчи поверх хуков и наоборот
//    а так же 2) давая возможность посмотреть дамп (общий листинг) всех патчей 
//    и хуков установленных с помощью этой библиотеки в конкретный момент времени.
////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ЛОГИРОВАНИЕ.
//
// по умолчанию в patcher_x86.dll логирование отключено, чтобы включить его,
// необходимо в той же папке создать файл patcher_x86.ini c единственной
// записью: Logging = 1 (Logging = 0 - отключает логирование снова)
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ПРАВИЛА ИСПОЛЬЗОВАНИЯ.
//
// 1) каждый мод должен 1 раз вызвать функцию GetPatcher(), сохранив результат
//  например: Patcher* _P = GetPatcher();
// 2) затем с помощью метода Pather::CreateInstance нужно создать  
// экземпляр PatсherInstance со своим уникальным именем
//  например: PatсherInstance* _PI = _P->CreateInstance("MyMod");
// 3)  затем использовать методы классов Patсher и PatсherInstance
// непосредственно для работы с патчами и хуками
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#define _byte_ unsigned __int8
#define _word_ unsigned __int16
#define _dword_ unsigned __int32


//макросы CALL_? позволяют вызывать произвольную функцию по определенному адресу
//используются в том числе для вызова функций
//полученных с помощью HiHook::GetDefaultFunc и HiHook::GetOriginalFunc
#define CALL_0(return_type, call_type, address) \
 ((return_type (call_type *)(void))address)()
#define CALL_1(return_type, call_type, address, a1) \
 ((return_type (call_type *)(_dword_))(address))((_dword_)(a1))
#define CALL_2(return_type, call_type, address, a1, a2) \
 ((return_type (call_type *)(_dword_,_dword_))(address))((_dword_)(a1),(_dword_)(a2))
#define CALL_3(return_type, call_type, address, a1, a2, a3) \
 ((return_type (call_type *)(_dword_,_dword_,_dword_))(address))((_dword_)(a1),(_dword_)(a2),(_dword_)(a3))
#define CALL_4(return_type, call_type, address, a1, a2, a3, a4) \
 ((return_type (call_type *)(_dword_,_dword_,_dword_,_dword_))(address))((_dword_)(a1),(_dword_)(a2),(_dword_)(a3),(_dword_)(a4))
#define CALL_5(return_type, call_type, address, a1, a2, a3, a4, a5) \
 ((return_type (call_type *)(_dword_,_dword_,_dword_,_dword_,_dword_))(address))((_dword_)(a1),(_dword_)(a2),(_dword_)(a3),(_dword_)(a4),(_dword_)(a5))
#define CALL_6(return_type, call_type, address, a1, a2, a3, a4, a5, a6) \
 ((return_type (call_type *)(_dword_,_dword_,_dword_,_dword_,_dword_,_dword_))(address))((_dword_)(a1),(_dword_)(a2),(_dword_)(a3),(_dword_)(a4),(_dword_)(a5),(_dword_)(a6))
#define CALL_7(return_type, call_type, address, a1, a2, a3, a4, a5, a6, a7) \
 ((return_type (call_type *)(_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_))(address))((_dword_)(a1),(_dword_)(a2),(_dword_)(a3),(_dword_)(a4),(_dword_)(a5),(_dword_)(a6),(_dword_)(a7))
#define CALL_8(return_type, call_type, address, a1, a2, a3, a4, a5, a6, a7, a8) \
 ((return_type (call_type *)(_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_))(address))((_dword_)(a1),(_dword_)(a2),(_dword_)(a3),(_dword_)(a4),(_dword_)(a5),(_dword_)(a6),(_dword_)(a7),(_dword_)(a8))
#define CALL_9(return_type, call_type, address, a1, a2, a3, a4, a5, a6, a7, a8, a9) \
 ((return_type (call_type *)(_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_))(address))((_dword_)(a1),(_dword_)(a2),(_dword_)(a3),(_dword_)(a4),(_dword_)(a5),(_dword_)(a6),(_dword_)(a7),(_dword_)(a8),(_dword_)(a9))
#define CALL_10(return_type, call_type, address, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10) \
 ((return_type (call_type *)(_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_))(address))((_dword_)(a1),(_dword_)(a2),(_dword_)(a3),(_dword_)(a4),(_dword_)(a5),(_dword_)(a6),(_dword_)(a7),(_dword_)(a8),(_dword_)(a9),(_dword_)(a10))
#define CALL_11(return_type, call_type, address, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11) \
 ((return_type (call_type *)(_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_))(address))((_dword_)(a1),(_dword_)(a2),(_dword_)(a3),(_dword_)(a4),(_dword_)(a5),(_dword_)(a6),(_dword_)(a7),(_dword_)(a8),(_dword_)(a9),(_dword_)(a10),(_dword_)(a11))
#define CALL_12(return_type, call_type, address, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12) \
 ((return_type (call_type *)(_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_))(address))((_dword_)(a1),(_dword_)(a2),(_dword_)(a3),(_dword_)(a4),(_dword_)(a5),(_dword_)(a6),(_dword_)(a7),(_dword_)(a8),(_dword_)(a9),(_dword_)(a10),(_dword_)(a11),(_dword_)(a12))
#define CALL_13(return_type, call_type, address, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13) \
 ((return_type (call_type *)(_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_))(address))((_dword_)(a1),(_dword_)(a2),(_dword_)(a3),(_dword_)(a4),(_dword_)(a5),(_dword_)(a6),(_dword_)(a7),(_dword_)(a8),(_dword_)(a9),(_dword_)(a10),(_dword_)(a11),(_dword_)(a12),(_dword_)(a13))
#define CALL_14(return_type, call_type, address, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14) \
 ((return_type (call_type *)(_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_))(address))((_dword_)(a1),(_dword_)(a2),(_dword_)(a3),(_dword_)(a4),(_dword_)(a5),(_dword_)(a6),(_dword_)(a7),(_dword_)(a8),(_dword_)(a9),(_dword_)(a10),(_dword_)(a11),(_dword_)(a12),(_dword_)(a13),(_dword_)(a14))
#define CALL_15(return_type, call_type, address, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15) \
 ((return_type (call_type *)(_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_))(address))((_dword_)(a1),(_dword_)(a2),(_dword_)(a3),(_dword_)(a4),(_dword_)(a5),(_dword_)(a6),(_dword_)(a7),(_dword_)(a8),(_dword_)(a9),(_dword_)(a10),(_dword_)(a11),(_dword_)(a12),(_dword_)(a13),(_dword_)(a14),(_dword_)(a15))
#define CALL_16(return_type, call_type, address, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16) \
 ((return_type (call_type *)(_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_))(address))((_dword_)(a1),(_dword_)(a2),(_dword_)(a3),(_dword_)(a4),(_dword_)(a5),(_dword_)(a6),(_dword_)(a7),(_dword_)(a8),(_dword_)(a9),(_dword_)(a10),(_dword_)(a11),(_dword_)(a12),(_dword_)(a13),(_dword_)(a14),(_dword_)(a15),(_dword_)(a16))
#define CALL_17(return_type, call_type, address, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17) \
 ((return_type (call_type *)(_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_,_dword_))(address))((_dword_)(a1),(_dword_)(a2),(_dword_)(a3),(_dword_)(a4),(_dword_)(a5),(_dword_)(a6),(_dword_)(a7),(_dword_)(a8),(_dword_)(a9),(_dword_)(a10),(_dword_)(a11),(_dword_)(a12),(_dword_)(a13),(_dword_)(a14),(_dword_)(a15),(_dword_)(a16),(_dword_)(a17))

#define CALL_VA(return_type, adress, a1, ...) \
 ((return_type (__cdecl *)(_dword_, ...))(adress))((_dword_)(a1), __VA_ARGS__)


// _bool_ - 4-х байтовый логический тип, как BOOL в Win32 API
// если очень захочется, можно заменить на BOOL или однобайтовый bool например
#define _bool_ __int32

// все адреса и часть указателей определены этим типом,
// если вам удобнее по-другому, можете заменить _ptr_ на любой другой тип, void* или int например
typedef _dword_ _ptr_;



// во всех структурах и интерфейсах библиотеки должно быть выравнивание - 4 байта
#pragma pack(push, 4)


//тип "переменная", используется для возвращаемых методами Patcher::VarInit и Patcher::VarFind значений.
class Variable
{
public:
	// возвращает значение 'переменной' (потокобезопасное обращение)
	virtual _dword_ __stdcall GetValue() = 0;
	// устанавливает значение 'переменной' (потокобезопасное обращение)
	virtual void __stdcall SetValue(_dword_ value) = 0;
	// возвращает указатель на значение (обращение к значению через указатель непотокобезопасно)
	virtual _dword_* __stdcall GetPValue() = 0;
};

// тип 'регистр флагов', размер - 32 бита 
// используется в HookContext
struct FlagsRegister
{
	_dword_ CF			: 1; //0
	_dword_ reserved_1	: 1; //1
	_dword_ PF			: 1; //2
	_dword_ reserved_3	: 1; //3
	_dword_ AF			: 1; //4
	_dword_ reserved_5	: 1; //5
	_dword_ ZF			: 1; //6
	_dword_ SF			: 1; //7
	_dword_ TF			: 1; //8
	_dword_ IF			: 1; //9
	_dword_ DF			: 1; //10
	_dword_ OF			: 1; //11
	_dword_ IOPL		: 2; //12-13
	_dword_ NT			: 1; //14
	_dword_ reserved_15	: 1; //15
	_dword_ RF			: 1; //16
	_dword_ VM			: 1; //17
	_dword_ AC			: 1; //18
	_dword_ VIF			: 1; //19
	_dword_ VIP			: 1; //20
	_dword_ ID			: 1; //21
	_dword_ reserved_22	: 1; //22
	_dword_ reserved_23	: 1; //23
	_dword_ reserved_24	: 1; //24
	_dword_ reserved_25	: 1; //25
	_dword_ reserved_26	: 1; //26
	_dword_ reserved_27	: 1; //27
	_dword_ reserved_28	: 1; //28
	_dword_ reserved_29	: 1; //29
	_dword_ reserved_30	: 1; //30
	_dword_ reserved_31	: 1; //31
};

// Структура HookContext
// используется в функциях сработавших по LoHook хуку
struct HookContext
{
 int eax; //регистр EAX, чтение/изменение
 int ecx; //регистр ECX, чтение/изменение
 int edx; //регистр EDX, чтение/изменение
 int ebx; //регистр EBX, чтение/изменение
 int esp; //регистр ESP, чтение/изменение
 int ebp; //регистр EBP, чтение/изменение
 int esi; //регистр ESI, чтение/изменение
 int edi; //регистр EDI, чтение/изменение

 _ptr_ return_address; //адрес возврата, чтение/изменение

 FlagsRegister flags; //регистр флагов, чтение/изменение
 // для языков программирования не поддерживающих битовые поля (напр. delphi)
 // flags может быть определен как _dword_ типа.


 // функция Push имеет аналогичное действие команде процессора PUSH для контекста LoHook хука
 // при использовании с контекстом хука установленного с помощью WriteLoHook или CreateLoHook
 // размер памяти которая может быть помещена в стек с помощью этой функции ограничен 128 байтами.
 // при использовании с контекстом хука установленного с помощью WriteLoHookEx или CreateLoHookEx
 // этот размер устанавливается произвольно при вызове WriteLoHookEx или CreateLoHookEx.
 inline void Push(int v)
 {
	 esp -= 4;
	 *(int*)(esp) = v;
 }

 // функция Pop имеет аналогичное действие команде процессора POP для контекста LoHook хука
 inline int Pop()
 {
	 int r = *(int*)(esp);
	 esp += 4;
	 return r;
 }
};



// значения возвращаемые функцией срабатываемой по LoHook хуку
#define EXEC_DEFAULT 1
#define NO_EXEC_DEFAULT 0
#define SKIP_DEFAULT 0


// значения возвращаеемые Patch::GetType()
#define PATCH_  0
#define LOHOOK_ 1
#define HIHOOK_ 2


// значения передаваемые PatcherInstance::Write() и PatcherInstance::CreatePatch()
#define DATA_ 0
#define CODE_ 1


// Абстрактный класс Patch
// создать экземпляр можно с
// помощью методов класса PatcherInstance
class Patch
{
public:
 // возвращает адрес по которому устанавливается патч
 virtual _ptr_ __stdcall GetAddress() = 0; 

 // возвращает размер патча
 virtual _dword_ __stdcall GetSize() = 0; 

 // возвращает уникальное имя экземпляра PatcherInstance, с помощью которого был создан патч
 virtual char const* __stdcall GetOwner() = 0;

 // возвращает тип патча
 // для не хука всегда PATCH_
 // для LoHook всегда LOHOOK_
 // для HiHook всегда HIHOOK_
 virtual int  __stdcall GetType() = 0;

 // возвращает true, если патч применен и false, если нет.
 virtual _bool_ __stdcall IsApplied() = 0;

 // применяет патч 
 // возвращает >= 0 , если патч/хук применился успешно
 // (возвращаемое значение является порядковым номером патча в последовательности
 // патчей, примененных по окрестности данного адреса, чем больше число, 
 // тем позднее был применен патч)
 // возвращает -2, если патч уже применен
 // Результат выполнения метода распространенно пишется в лог
 virtual _bool_ __stdcall Apply() = 0;

    // ApplyInsert применяет патч с указанием порядкового номера в
 // последовательности патчей, примененных по этому адресу.
 // возвращаемые значения аналогичны соответсвующим в Patch::Apply
 // функции ApplyInsert можно аргументом передать значение, возвращаемое 
 // функцией Undo, чтобы применить патч в то же место, на котором тот был до отменения.
 virtual _bool_ __stdcall ApplyInsert(int zorder) = 0;

 /////////////////////////////////////////////////////////////////////////////////////////////////////
 // Метод Undo
 // Отменяет патч(хук) (в случае если патч применен последним - восстанавливает затертый код)
 // Возвращает число >= 0, если патч(хук) был отменен успешно 
 // (возвращаемое значение является номером патча в последовательности
 // патчей, примененных по данному адресу, чем больше число, 
 // тем позднее был применен патч)
 // Возвращает -2, если патч и так уже был отменен (не был применен)
 // Результат выполнения метода распространенно пишется в лог
 virtual _bool_ __stdcall Undo() = 0;


 /////////////////////////////////////////////////////////////////////////////////////////////////////
 // Метод Destroy
 // Деструктор
 // Отменяет и безвозвратно уничтожает патч/хук
 // возвращает всегда 1 (для совместимости с более ранними версиями библиотеки)
 // Результат уничтожения распространенно пишется в лог
 virtual _bool_ __stdcall Destroy() = 0;

 /////////////////////////////////////////////////////////////////////////////////////////////////////
 // Метод GetAppliedBefore
 // возвращает патч примененный перед данным
 // возвращает NULL если данный патч применен первым
 virtual Patch* __stdcall GetAppliedBefore() = 0;

 /////////////////////////////////////////////////////////////////////////////////////////////////////
 // Метод GetAppliedAfter
 // возвращает патч примененный после данного
 // возвращает NULL если данный патч применен последним
 virtual Patch* __stdcall GetAppliedAfter() = 0;

};


// Абстрактный класс LoHook (унаследован от Patch, т.е. по сути лоу-хук является патчем)
// создать экземпляр можно с
// помощью методов класса PatcherInstance
class LoHook : public Patch
{
};


typedef int(__stdcall *_LoHookFunc_)(LoHook*, HookContext*);


// значения передаваемые как аргумент hooktype в PatcherInstance::WriteHiHook и PatcherInstance::CreateHiHook
#define CALL_ 0
#define SPLICE_ 1
#define FUNCPTR_ 2

// значения передаваемые как аргумент subtype в PatcherInstance::WriteHiHook и PatcherInstance::CreateHiHook
#define DIRECT_  0
#define EXTENDED_ 1
#define SAFE_  2

// значения передаваемые как аргумент calltype в PatcherInstance::WriteHiHook и PatcherInstance::CreateHiHook
#define ANY_  0
#define STDCALL_ 0
#define THISCALL_ 1
#define FASTCALL_ 2 
#define CDECL_  3
#define FASTCALL_1 1 

// Абстрактный класс HiHook (унаследован от Patch, т.е. по сути хай-хук является патчем)
// создать экземпляр можно с помощью методов класса PatcherInstance
class HiHook : public Patch
{
public:
 // возвращает указатель на функцию (на мост к функции в случае SPLICE_),
 // замещенную хуком
 // Внимание! Вызывая функцию для не примененного хука, можно получить
 // неактуальное (но рабочее) значение.
 virtual _ptr_ __stdcall GetDefaultFunc() = 0;

 // возвращает указатель на оригинальную функцию (на мост к функции в случае SPLICE_),
 // замещенную хуком(хуками) по данному адресу
 // (т.е. возвращает GetDefaultFunc() для первого примененного хука по данному адресу)
 // Внимание! Вызывая функцию для не примененного хука, можно получить
 // неактуальное (но рабочее) значение.
 virtual _ptr_ __stdcall GetOriginalFunc() = 0;

 // возвращает адрес возврата в оригинальный код
 // можно использовать внутри хук-функции
 // SPLICE_ EXTENDED_ или SAFE_ хука, чтобы узнать откуда она была вызвана
 // для SPLICE_ DIRECT_ хука функция возвращает всегда 0 (т.е. для DIRECT_ хука возможности узнать адрес возврата через нее - нет)
 virtual _ptr_ __stdcall GetReturnAddress() = 0;


 //# ver 2.1
 // устанавливает значение пользовательских данных хука
 virtual void __stdcall SetUserData(_dword_ data) = 0;
 // возвращает значение пользовательских данных хука
 // если не задано пользователем, то равно 0
 virtual _dword_ __stdcall GetUserData() = 0;
};


// Абстрактный класс PatcherInstance
// создать/получить экземпляр можно с помощью методов CreateInstance и GetInstance класса Patcher
// непосредственно позволяет создавать/устанавливать патчи и хуки в код,
// добавляя их в дерево всех патчей/хуков, созданных библиотекой patcher_x86.dll
class PatcherInstance
{
public:
 ////////////////////////////////////////////////////////////
 // Метод WriteByte
 // пишет однбайтовое число по адресу address
 // (создает и применяет DATA_ патч)
 // Возвращает указатель на патч
 virtual Patch* __stdcall WriteByte(_ptr_ address, int value) = 0;
 
 ////////////////////////////////////////////////////////////
 // Метод WriteWord
 // пишет двухбайтовое число по адресу address
 // (создает и применяет DATA_ патч)
 // Возвращает указатель на патч
 virtual Patch* __stdcall WriteWord(_ptr_ address, int value) = 0;
 
 ////////////////////////////////////////////////////////////
 // Метод WriteDword
 // пишет четырехбайтовое число по адресу address
 // (создает и применяет DATA_ патч)
 // Возвращает указатель на патч
 virtual Patch* __stdcall WriteDword(_ptr_ address, int value) = 0;
 
 ////////////////////////////////////////////////////////////
 // Метод WriteJmp
 // пишет jmp to опкод по адресу address
 // (создает и применяет CODE_ патч)
 // Возвращает указатель на патч
 // патч закрывает целое количество опкодов,
 // т.е. размер патча >= 5, разница заполнятеся NOP'ами. 
 virtual Patch* __stdcall WriteJmp(_ptr_ address, _ptr_ to) = 0;
 
 ////////////////////////////////////////////////////////////
 // Метод WriteHexPatch
 // пишет по адресу address позледовательность байт,
 // определенную hex_str
 // (создает и применяет DATA_ патч)
 // Возвращает указатель на патч
 // hex_str - си-строка может содержать шестнадцатеричные цифры
 // 0123456789ABCDEF (только верхний регистр!) остальные символы 
 // при чтении методом hex_str игнорируются(пропускаются)
 // удобно использовать в качестве аргумента этого метода
 // скопированное с помощью Binary copy в OllyDbg
 /* пример:
   pi->WriteHexPatch(0x57b521, "6A 01  6A 00");
 */
 virtual Patch* __stdcall WriteHexPatch(_ptr_ address, char const* hex_str) = 0;
 
 ////////////////////////////////////////////////////////////
 // Метод WriteCodePatchVA
 // в оригинальном виде применение метода не предполагается,
 // смотрите (ниже) описание метода-оболочки WriteCodePatch
 virtual Patch* __stdcall WriteCodePatchVA(_ptr_ address, char const* format, _dword_* va_args) = 0;
 
 ////////////////////////////////////////////////////////////
 // Метод WriteLoHook
 // создает по адресу address низкоуровневый хук (CODE_ патч) и применяет его
 // возвращает указатель на хук
 // func - функция вызываемая при срабатывании хука
 // должна иметь вид int __stdcall func(LoHook* h, HookContext* c);
 // в HookContext* c передаются для чтения/изменения 
 // регистры процессора и адрес возврата
 // если func возвращает EXEC_DEFAULT, то 
 // после завершения func выполняется затертый хуком код.
 // если - SKIP_DEFAULT - затертый код не выполняется
 //
 // ВНИМАНИЕ! 
 // размер памяти, которая может быть помещена в стек контекста
 // с помощью использования c->esp и с->Push, ограничен 128 байтами.
 // если требуется иное ограничение используйте метод WriteLoHookEx или CreateLoHookEx.
 virtual LoHook* __stdcall WriteLoHook(_ptr_ address, _LoHookFunc_ func) = 0;
 
 ////////////////////////////////////////////////////////////
 // Метод WriteHiHook
 // создает по адресу address высокоуровневый хук и применяет его
 // возвращает указатель на хук
 //
 // new_func - функция замещающая оригинальную
 //
 // hooktype - тип хука:
 //  CALL_ -  хук НА ВЫЗОВ функции по адресу address
 //     поддерживаются опкоды E8 и FF 15, в остальных случаях хук не устанавливается
 //     и в лог пишется информация об этой ошибке
 //  SPLICE_ - хук непосредственно НА САМУ ФУНКЦИЮ по адресу address
 //  FUNCPTR_ - хук на функцию в указателе (применяется редко, в основном для хуков в таблицах импорта)
 //
 // subtype - подтип хука:
 //  DIRECT_ - new_func имеет тот же вид что и
 //     оригинальная замещаемая функция
 //     примечание: вместо __thiscall f(this) 
 //        можно использовать __fastcal f(this_)
 //        вместо __thiscall f(this, ...)  можно использовать 
 //        __fastcall f(this_, no_used_edx, ...) 
 //  EXTENDED_ - функции new_func первым стековым аргументом передается
 //     указатель на экземпляр HiHook и, в случае 
 //     соглашений исходной ф-ии __thiscall и __fastcall
 //     регистровые аргументы передаются стековыми вторыми 
 // Таким образом функция new_func должна иметь вид
 // ? __stdcall new_func(HiHook* hook, ?) для ? ? orig(?)
 //
 // ВНИМАНИЕ! EXTENDED_ FASTCALL_ поддерживает только функции с 2-мя и более аргументами
 // для __fastcall c 1 аргументом используйте EXTENDED_ FASTCALL_1 / EXTENDED_ THISCALL_
 //
 //   SAFE_ - то же самое что и EXTENDED_, однако перед вызовом (на время вызова) GetDefaultFunc() восстанавливаются
 //    значения регистров процессора EAX, ECX (если не FASTCALL_ и не THISCALL_),
 //    EDX (если не FASTCALL_), EBX, ESI, EDI, бывшие на момент вызова замещенной функции
 //
 //  в подавляющем большинстве случаев удобнее использовать EXTENDED_
 //  но DIRECT_ выполняется быстрее из-за отсутвия моста к новой замещающей функции
 //
 // calltype - соглашение о вызове оригинальной замещаемой ф-ии:
 //  STDCALL_
 //  THISCALL_
 //  FASTCALL_
 //  CDECL_
 // необходимо верно указывать соглашение для того чтобы EXTENDED_ хук правильно
 // построил мост к новой замещающей функции
 //
 // CALL_, SPLICE_ хук является CODE_ патчем
 // FUNCPTR_ хук является DATA_ патчем
 //
 virtual HiHook* __stdcall WriteHiHook(_ptr_ address, int hooktype, int subtype, int calltype, void* new_func) = 0;

 ///////////////////////////////////////////////////////////////////
 // Методы Create...
 // создают патч/хук так же как и соответствующие методы Write...,
 // но НЕ ПРИМЕНЯЮТ его
 // возвращают указатель на патч/хук
 virtual Patch* __stdcall CreateBytePatch(_ptr_ address, int value) = 0;
 virtual Patch* __stdcall CreateWordPatch(_ptr_ address, int value) = 0;
 virtual Patch* __stdcall CreateDwordPatch(_ptr_ address, int value) = 0;
 virtual Patch* __stdcall CreateJmpPatch(_ptr_ address, _ptr_ to) = 0;
 virtual Patch* __stdcall CreateHexPatch(_ptr_ address, char const* hex_str) = 0;
 virtual Patch* __stdcall CreateCodePatchVA(_ptr_ address, char const* format, _dword_* va_args) = 0;
 virtual LoHook* __stdcall CreateLoHook(_ptr_ address, void* func) = 0;
 virtual HiHook* __stdcall CreateHiHook(_ptr_ address, int hooktype, int subtype, int calltype, void* new_func) = 0;
 
 ////////////////////////////////////////////////////////////
 // Метод ApplyAll
 // применяет все патчи/хуки, созданные этим экземпляром PatcherInstance
 // всегда возвращает 1 (для совместимости с более ранними версиями библиотеки)
 // (см. Patch::Apply)
 virtual _bool_ __stdcall ApplyAll() = 0;

 ////////////////////////////////////////////////////////////
 // Метод UndoAll
 // отменяет все патчи/хуки, созданные этим экземпляром PatcherInstance
 // т.е. для каждого из патчей/хуков вызывает метод Undo
 // всегда возвращает 1 (для совместимости с более ранними версиями библиотеки)
 // (см. Patch::Undo)
 virtual _bool_ __stdcall UndoAll() = 0;

 ////////////////////////////////////////////////////////////
 // Метод DestroyAll
 // отменяет и безвозвратно уничтожает все патчи/хуки, созданные этим экземпляром PatcherInstance
 // т.е. для каждого из патчей/хуков вызывает метод Destroy
 // всегда возвращает 1 (для совместимости с более ранними версиями библиотеки)
 // (см. Patch::Destroy)
 virtual _bool_ __stdcall DestroyAll() = 0;

 // в оригинальном виде применение метода не предполагается,
 // смотрите (ниже) описание метода-оболочки WriteDataPatch
 virtual Patch* __stdcall WriteDataPatchVA(_ptr_ address, char const* format, _dword_* va_args);
 // в оригинальном виде применение метода не предполагается,
 // смотрите (ниже) описание метода-оболочки WriteDataPatch
 virtual Patch* __stdcall CreateDataPatchVA(_ptr_ address, char const* format, _dword_* va_args);


 // Метод GetLastPatchAt
 // возвращает NULL, если в окрестности адреса address не был применен ни один патч/хук,
 // созданный данным экземпляром PatcherInstance
 // иначе возвращает последний примененый патч/хук в окрестности адреса address,
 // созданный данным экземпляром PatcherInstance
 virtual Patch* __stdcall GetLastPatchAt(_ptr_ address) = 0;

 // Метод UndoAllAt
 // отменяет патчи примененные данным экземпляром PatcherInstance
 // в окрестности адреса address 
 // всегда возвращает 1 (для совместимости с более ранними версиями библиотеки)
 // (см. Patch::Undo)
 virtual _bool_ __stdcall UndoAllAt(_ptr_ address) = 0;

 // Метод GetFirstPatchAt
 // возвращает NULL, если в окрестности адреса address не был применен ни один патч/хук,
 // созданный данным экземпляром PatcherInstance
 // иначе возвращает первый примененый патч/хук в окрестности адреса address,
 // созданный данным экземпляром PatcherInstance
 virtual Patch* __stdcall GetFirstPatchAt(_ptr_ address) = 0;


 ////////////////////////////////////////////////////////////
 // Метод Write
 // пишет по адресу address данные/код из памяти по адресу data размером size байт 
 // если is_code == 1, то создается и пишется CODE_ патч, если 0 - DATA_ патч.
 // Возвращает указатель на патч
 virtual Patch* __stdcall Write(_ptr_ address, _ptr_ data, _dword_ size, _bool_ is_code = 0) = 0;
 ///////////////////////////////////////////////////////////////////
 // Метод CreatePatch
 // создаёт патч так же как и метод Write,
 // но НЕ ПРИМЕНЯЕТ его
 // возвращают указатель на патч
 virtual Patch* __stdcall CreatePatch(_ptr_ address, _ptr_ data, _dword_ size, _bool_ is_code = 0) = 0;


 //## ver 2.1
  ////////////////////////////////////////////////////////////
 // Метод WriteLoHookEx
 // аналогичен методу WriteLoHook, но имеет дополнительный аргумент
 // stack_delta - размер памяти который можно поместить в стэк контекста
 // используя HookContext::esp и HookContext::Push внутри func.
 //
 // Возвращают указатель на LoHook хук
 virtual LoHook* __stdcall WriteLoHookEx(_ptr_ address, void* func, _dword_ stack_delta) = 0;
 // Метод CreateLoHookEx
 // создает хук так же как и WriteLoHookEx,
 // но НЕ ПРИМЕНЯЕТ его.
 // Возвращают указатель на LoHook хук
 virtual LoHook* __stdcall CreateLoHookEx(_ptr_ address, void* func, _dword_ stack_delta) = 0;


 // в оригинальном виде применение метода не предполагается,
 // смотрите (ниже) описание метода-оболочки WriteHexHook
 virtual LoHook* __stdcall WriteHexHookVA(_ptr_ address, _bool_ exec_default, char const* hex_str, _dword_* va_args) = 0;
 // в оригинальном виде применение метода не предполагается,
 // смотрите (ниже) описание метода-оболочки CreateHexHook
 virtual LoHook* __stdcall CreateHexHookVA(_ptr_ address, _bool_ exec_default, char const* hex_str, _dword_* va_args) = 0;



 // метод BlockAt устанавливает блок на определенный адрес (именно на конкретный адрес а не на окрестность)
 // для данного экземпляра PatcherInstance
 // после чего данный экземпляр PatcherInstance не может применять 
 // патчи по этому адресу
 virtual void __stdcall BlockAt(_ptr_ address) = 0;

 //## ver 2.6

 // метод BlockAllExceptVA устанавливает блок на все адреса кроме указанных в va_args
 // так же как и метод BlockAt оперирует конкретными адресами а не окрестностями
 // для данного экземпляра PatcherInstance
 // после чего данный экземпляр PatcherInstance не может применять 
 // патчи по всем адресам кроме указанных.
 // список адресов в va_args должен заканчиваться 0 (нулем)
 virtual void __stdcall BlockAllExceptVA(_dword_ *va_args) = 0;


 //## ver 4.0
 // в оригинальном виде применение метода не предполагается,
 // смотрите (ниже) описание метода-оболочки WriteAsmPatch
 virtual Patch* __stdcall WriteAsmPatchVA(_ptr_ address, _dword_* va_args) = 0;

 // в оригинальном виде применение метода не предполагается,
 // смотрите (ниже) описание метода-оболочки CreateAsmPatch
 virtual Patch* __stdcall CreateAsmPatchVA(_ptr_ address, _dword_* va_args) = 0;

 // в оригинальном виде применение метода не предполагается,
 // смотрите (ниже) описание метода-оболочки WriteAsmHook
 virtual LoHook* __stdcall WriteAsmHookVA(_ptr_ address, _dword_* va_args) = 0;

 // в оригинальном виде применение метода не предполагается,
 // смотрите (ниже) описание метода-оболочки CreateAsmHook
 virtual LoHook* __stdcall CreateAsmHookVA(_ptr_ address, _dword_* va_args) = 0;


 //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 // WriteAsmPatch пишет патч по адресу address
 // возвращаeт указатель на патч
 // аргументы ... - это строки, содержащие код на ассемблере
 // распознаются все инструкции, распознаваемые OllyDbg 1.04 (вплоть до MMX и amd 3DNow! включительно)
 // ВНИМАНИЕ! в отличие от OllyDbg, целые числа не имеющие префикса 0x или постфикса h читаются как ДЕСЯТЕРИЧНЫЕ!
 // поэтому не забывайте писать шеснадцатеричные числа явно
 // В одной строке может быть несколько инструкций, разделенных символом ';'
 // Код на ассемблере может содержать метки;
 // объявление метки - label_name: (имя метки, двоеточие),
 // использование label_name (имя без двоеточия);
 // максимальная длина имени метки - 39 символов, имя может содержать буквы, цифры, символы '_' и '.';
 // код может содержать псевдоинструкцию times (пишет заданное количество раз следующую инструкцию)
 // например, результатом "times 3 nop" будет код 90 90 90
 // код может содержать псевдоинструкцию _ExecCopy <адрес>, <размер> (пишет код из памяти по адресу <адрес> размером <размер>)
 // код может содержать псевдоинструкции db <число>, dw <число>, dd <число или метка>.
 // Строка может содержать формат-символы %d. В этом случае за строкой должно следовать соответствующее количество четырехбайтовых чисел (знаковые/беззнаковые/адреса/...)
 // ВНИМАНИЕ! последним аргументом (строкой) должен обязательно быть 0 (NULL)!
 // абстрактный пример: WriteAsmPatch(0x112233, "begin: call %d", MyFunc, "jmp begin", "jne dword [%d]", 0xAABBCC, 0);
 inline Patch* WriteAsmPatch(_ptr_ address, ...)
 {
	 return WriteAsmPatchVA(address, (_dword_*)((_ptr_)&address + 4));
 }
 ////////////////////////////////////////////////////////////
 // Метод CreateAsmPatch
 // создает патч так же как и метод WriteCodePatch,
 // но не применяет его
 // возвращаeт указатель на патч
 inline Patch* CreateAsmPatch(_ptr_ address, ...)
 {
	 return CreateAsmPatchVA(address, (_dword_*)((_ptr_)&address + 4));
 }

 ////////////////////////////////////////////////////////////
 // WriteAsmHook пишет по адресу address примитивный хук
 // а именно LoHook без вызова высокоуровневой функции.
 // тело хука пишется прямо в вызове CreateHexHook или WriteHexHook
 // таким же образом как пишется патч с помощью WriteAsmPatch (см. WriteAsmPatch)
 // В отличие метода WriteAsmPatch здесь код может содержать (а может не содержать) псевдокоманду _ExecDefault,
 // котроая выполняет затертый хуком код
 // ВНИМАНИЕ! в коде не может быть больше одной псевдокоманды _ExecDefault
 // возвращает указатель на LoHook хук
 // ВНИМАНИЕ! последним аргументом (строкой) должен обязательно быть 0 (NULL)!
 // абстрактный пример: WriteAsmHook(0x112233, "cmp eax, 0; jne SkipDefault; _ExecDefault; jmp End; SkipDefault: mov ecx, 2; End: retn", 0);
 inline LoHook* WriteAsmHook(_ptr_ address, ...)
 {
	 return WriteAsmHookVA(address, (_dword_*)((_ptr_)&address + 4));
 }

 ////////////////////////////////////////////////////////////
 // Метод CreateAsmHook
 // создает хук так же как и WriteAsmHook,
 // но НЕ ПРИМЕНЯЕТ его.
 // Возвращают указатель на LoHook хук
 inline LoHook* CreateAsmHook(_ptr_ address, ...)
 {
	 return CreateAsmHookVA(address, (_dword_*)((_ptr_)&address + 4));
 }



//WriteHexHook пишет по адресу address самый примитивный хук
//а именно LoHook без вызова высокоуровневой функции.
//тело хука пишется прямо в вызове CreateHexHook или WriteHexHook
//таким же образом как пишется патч с помощью WriteCodePatch (см. WriteCodePatch)
//
//exec_default - выполнять ли затертый хуком код после выполнения тела хука
//возвращает указатель на LoHook хук
 inline LoHook* WriteHexHook(_ptr_ address, _bool_ exec_default, char const* format, ...)
 {
  return WriteHexHookVA(address, exec_default, format, (_dword_*)((_ptr_)&format + 4));
 }
 // Метод CreateHexHook
 // создает хук так же как и WriteHexHook,
 // но НЕ ПРИМЕНЯЕТ его.
 // Возвращают указатель на LoHook хук
 inline LoHook* CreateHexHook(_ptr_ address, _bool_ exec_default, char const* format, ...)
 {
  return CreateHexHookVA(address, exec_default, format, (_dword_*)((_ptr_)&format + 4));
 }


 ////////////////////////////////////////////////////////////
 // Метод WriteCodePatch
 // пишет по адресу address позледовательность байт,
 // определенную format и ...
 // (создает и применяет CODE_ патч)
 // Возвращает указатель на патч
 // format - си-строка может содержать шестнадцатеричные цифры
 // 0123456789ABCDEF (только верхний регистр!),
 // а так же специальные формат-символы (нижний регистр!):
 // %b - (byte) пишет однобайтовое число из ...
 // %w - (word) пишет двухбайтовое число из ...
 // %d - (dword) пишет четырехбайтовое число из ...
 // %j - пишет jmp на адрес из ...
 // %с - пишет сall ...
 // %m - копирует код по адресу ... размером ... (т.е. читает 2 аргумента из ...)
 //      копирование происходит посредством MemCopyCodeEx (см. описание)
 // %% - пишет строку с формат-символами из ... 
 // %o - (offset) помещает по адресу из аргумента смещение позиции в
 //      Complex коде,  относительно начала Complex кода.
 // %n - пишет nop опкоды, количеством равным ...
 // #0: - #9: -устанавливает метку (от 0 до 9) к которой можно перейти с помощью #0 - #9
 // #0 -  #9  -пишет отностельный адрес после опкодов EB, 70 - 7F, E8, E9, 0F80 - 0F8F
 //      соответствующей метки; после других опкодов ничего не пишет
 // ~b - берет из ... абсолютный адрес и пишет относительное смещение до него
 //      размером в 1 байт (используется для опкодов EB, 70 - 7F)
 // ~d - берет из ... абсолютный адрес и пишет относительное смещение до него
 //      размером в 4 байта (используется для опкодов E8, E9, 0F 80 - 0F 8F)
 // %. - ничего не делает ( как и любой другой не объявленный выше символ после % ) 
 // абстрактный пример:
 // Patch* p = pi->WriteCodePatch(address,
 //  "#0: %%",
 //  "B9 %d %%", this,     // mov ecx, this  // 
 //  "BA %d %%", this->context,   // mov edx, context  // 
 //  "%c %%", func,      // call func  // 
 //  "83 F8 01 %%",      // cmp eax, 1
 //  "0F 85 #7 %%",       // jne long to label 7 (if func returns 0)
 //  "83 F8 02 %%",      // cmp eax, 2
 //  "0F 85 ~d %%", 0x445544,   // jne long to 0x445544 (if func returns 0)
 //  "EB #0 %%",       // jmp short to label 0
 //  "%m %%", address2, size,   // exec  code copy from address2
 //  "#7: FF 25 %d %.", &return_address); // jmp [&return_address]
 inline Patch* WriteCodePatch(_ptr_ address, char const* format, ...)
 {
  return WriteCodePatchVA(address, format, (_dword_*)((_ptr_)&format + 4));
 }
 
 ////////////////////////////////////////////////////////////
 // Метод CreateCodePatch
 // создает патч так же как и метод WriteCodePatch,
 // но не применяет его
 // возвращаeт указатель на патч
 inline Patch* CreateCodePatch(_ptr_ address, char const* format, ...)
 {
  return CreateCodePatchVA(address, format, (_dword_*)((_ptr_)&format + 4));
 }


 ////////////////////////////////////////////////////////////
 // Метод WriteDataPatch
 // пишет по адресу address позледовательность байт,
 // определенную format и ...
 // (создает и применяет DATA_ патч)
 // Возвращает указатель на патч
 // format - си-строка может содержать шестнадцатеричные цифры
 // 0123456789ABCDEF (только верхний регистр!),
 // а так же специальные формат-символы (нижний регистр!):
 // %b - (byte) пишет однобайтовое число из ...
 // %w - (word) пишет двухбайтовое число из ...
 // %d - (dword) пишет четырехбайтовое число из ...
 // %m - копирует данные по адресу ... размером ... (т.е. читает 2 аргумента из ...)
 // %% - пишет строку с формат-символами из ... 
 // %o - (offset) помещает по адресу из аргумента смещение позиции в
 //      Complex коде,  относительно начала Complex кода.
 // %. - ничего не делает ( как и любой другой не объявленный выше символ после % ) 
 // абстрактный пример:
 // Patch* p = pi->WriteDataPatch(address,
 //  "FF FF FF %d %%", var, 
 //  "%m %%", address2, size, 
 //  "AE %.");
 inline Patch* WriteDataPatch(_ptr_ address, char const* format, ...)
 {
  return WriteDataPatchVA(address, format, (_dword_*)((_ptr_)&format + 4));
 }
 
 ////////////////////////////////////////////////////////////
 // Метод CreateDataPatch
 // создает патч так же как и метод WriteDataPatch,
 // но не применяет его
 // возвращаeт указатель на патч
 inline Patch* CreateDataPatch(_ptr_ address, char const* format, ...)
 {
  return CreateDataPatchVA(address, format, (_dword_*)((_ptr_)&format + 4));
 }
 
 inline Patch* __stdcall WriteDword(_ptr_ address, char const* value)
 {
  return WriteDword(address, (int)value);
 }

};

// Класс Patcher
class Patcher
{
public:
 // основные методы:

 ///////////////////////////////////////////////////
 // Метод CreateInstance
 // создает экземпляр класса PatcherInstance, который 
 // непосредственно позволяет создавать патчи и хуки и
 // возвращает указатель на этот экземпляр.
 // owner - уникальное имя экземпляра PatcherInstance
 // метод возвращает NULL, если экземпляр с именем owner уже создан
 // если owner == NULL или owner == "" то 
 // экземпляр PatcherInstance будет создан с именем модуля из
 // которого была вызвана функция.
 virtual PatcherInstance* __stdcall CreateInstance(char const* owner) = 0;
 
 ///////////////////////////////////////////////////
 // Метод GetInstance
 // Возвращает указатель на экземпляр PatcherInstance
 // с именем owner.
 // метод возвращает NULL в случае, если 
 // экземпляр с именем owner не существует (не был создан)
 // в качестве аргумента можно передавать имя модуля.
 // Используется для :
 // - проверки активен ли некоторый мод, использующий patcher_x86.dll
 // - получения доступа ко всем патчам и хукам некоторого мода,
 //   использующего patcher_x86.dll
 virtual PatcherInstance*  __stdcall GetInstance(char const* owner) = 0;
 
 ///////////////////////////////////////////////////
 // Метод GetLastPatchAt
 // возвращает NULL, если в окрестности адреса address не был применен ни один патч/хук
 // иначе возвращает последний примененый патч/хук в окрестности адреса address
 // последовательно пройтись по всем патчам в заданной окрестности 
 // используя этот метод и Patch::GetAppliedBefore
 virtual Patch* __stdcall GetLastPatchAt(_ptr_ address);
 
 ///////////////////////////////////////////////////
 // Метод UndoAllAt
 // отменяет все патчи/хуки в окрестности адреса address
 // всегда возвращает 1 (для совместимости с более ранними версиями библиотеки)
 virtual Patch* __stdcall UndoAllAt(_ptr_ address);
 
 ///////////////////////////////////////////////////
 // Метод SaveDump
 // сохраняет в файл с именем file_name
 // - количество и имена всех экземпляров PatcherInstance
 // - количество всех примененных патчей/хуков
 // - список всех примененных патчей и хуков с их адресами установки, размерами, глобальной очередностью применения, собственниками (именами PatcherInstance)
 virtual void __stdcall SaveDump(char const* file_name) = 0;
 
 ///////////////////////////////////////////////////
 // Метод SaveLog
 // сохраняет в файл с именем file_name лог 
 // если логирование выключено в логе будет 0 записей.
 // включить логирование можно создав в директории библиотеки
 // текстовый файл patcher_x86.ini c cодержимым: Logging = 1
 virtual void __stdcall SaveLog(char const* file_name) = 0;
 
 ///////////////////////////////////////////////////
 // Метод GetMaxPatchSize
 // Библиотека patcher_x86.dll накладывает некоторые ограничения
 // на максимальный размер патча,
 // какой - можно узнать с помощью метода GetMaxPatchSize
 // (на данный момент это 262144 байт, т.е. дохрена :) )
 virtual int __stdcall GetMaxPatchSize() = 0;
 
 // дополнительные методы:

 ///////////////////////////////////////////////////
 // Метод WriteComplexDataVA
 // в оригинальном виде применение метода не предполагается,
 // смотрите (ниже) описание метода-оболочки WriteComplexString
 virtual int __stdcall WriteComplexDataVA(_ptr_ address, char const* format, _dword_* args) = 0;
 
 ///////////////////////////////////////////////////
 // метод GetOpcodeLength
 // т.н. дизассемблер длин опкодов
 // возвращает длину в байтах опкода по адресу p_opcode
 // возвращает 0, если опкод неизвестен
 virtual int __stdcall GetOpcodeLength(_ptr_ p_opcode) = 0;
 
 ///////////////////////////////////////////////////
 // метод MemCopyCode
 // копирует код из памяти по адресу src в память по адресу dst
 // MemCopyCode копирует всегда целое количество опкодов размером >= size. Будьте внимательны!
 // возвращает размер скопированного кода.
 // отличается действием от простого копирования памяти тем,
 // что корректно копирует опкоды E8 (call), E9 (jmp long), 0F80 - 0F8F (j** long)
 // c относительной адресацией не сбивая в них адреса, если инструкции 
 // направляют за пределы копируемого блокая.
 // 
 virtual int __stdcall MemCopyCode(_ptr_ dst, _ptr_ src, _dword_ size) = 0;
 
 ///////////////////////////////////////////////////
 // Метод GetFirstPatchAt
 // возвращает NULL, если в окрестности адреса address не был применен ни один патч/хук
 // иначе возвращает первый примененый патч/хук в окрестности адреса address
 // последовательно пройтись по всем патчам в заданной окрестности 
 // используя этот метод и Patch::GetAppliedAfter
 virtual Patch* __stdcall GetFirstPatchAt(_ptr_ address);
 
 ///////////////////////////////////////////////////
 // метод MemCopyCodeEx
 // копирует код из памяти по адресу src в память по адресу dst
 // возвращает размер скопированного кода.
 // отличается от MemCopyCode тем,
 // что корректно копирует опкоды EB (jmp short), 70 - 7F (j** short)
 // c относительной адресацией не сбивая в них адреса, если инструкции 
 // направляют за пределы копируемого блокая (в этом случае они заменяются на
 // соответствующие E9 (jmp long), 0F80 - 0F8F (j** long) опкоды.
 // Внимание! Из-за этого размер скопированного кода может оказаться значительно 
 // больше копируемого.
 virtual int __stdcall MemCopyCodeEx(_ptr_ dst, _ptr_ src, _dword_ size) = 0;
 
 
 // ver 2.3
 // Метод VarInit
 // инициализирует "переменную" c именем name и устанавливает значение "переменной" равным value
 // если "переменная" с таким именем уже существует, то просто устанавливает ее значение равным value
 // возвращает указатель на "переменную" в случае успеха и NULL в противном случае
 virtual Variable* __stdcall VarInit(char const* name, _dword_ value) = 0;
 // Метод VarFind
 // возвращает указатель на "переменную" с именем name, если такая была инициализированна
 // если нет, возвращает NULL
 virtual Variable* __stdcall VarFind(char const* name) = 0;


 // ver 2.6
 // Метод PreCreateInstance
 // Создает неполноценный экземпляр PatcherInstance с указанным именем.
 // PatcherInstance созданный таким образом не может создавать патчи.
 // Этот неполноценный экземпляр используется для применения методов PatcherInstance::BlockAt и PatcherInstance::BlockAllExceptVA
 // чтобы можно было заблокировать адреса до того как данный PatcherInstance будет полноценно создан с помощью CreateInstance
 virtual PatcherInstance* __stdcall PreCreateInstance(char const* name) = 0;


 // ver 4.1
 // описание в разработке...
 virtual int __stdcall WriteAsmCodeVA(_ptr_ address, _dword_* args) = 0;
 virtual _ptr_ __stdcall CreateCodeBlockVA(_dword_* args) = 0;

 
 // метод VarGetValue возвращает значение "переменной" c именем name
 // если "переменная" с таким именем не была инициализированна, возвращает default_value
 template<typename ValueType>
 inline ValueType VarGetValue(char const* name, ValueType default_value)
 {
	 if (sizeof(ValueType) > 4) return default_value;
	 Variable* v = VarFind(name);
	 if (v == NULL) return default_value;
	 return (ValueType)v->GetValue();
 }

 // метод VarValue возвращает ссылку на значение "переменной" c именем name
 // если "переменная" с таким именем не была инициализированна, инициализирует ее и устанавливает значение равным 0
 // внимание, обращение к значению переменной по ссылке непотокобезопасно
 template<typename ValueType>
 inline ValueType& VarValue(char const* name)
 {
	 if (sizeof(ValueType) > 4) __asm{__asm int 3};
	 
	 Variable* v = VarFind(name);
	 if (v == NULL) v = VarInit(name, 0);
	 
	 if (v == NULL) __asm{__asm int 3};
	 
	 return (ValueType&)*v->GetPValue();
 }



 ////////////////////////////////////////////////////////////////////
 // метод WriteComplexData
 // является более удобным интерфейсом  
 // метода WriteComplexDataVA
 // этот метод определен здесь а не в библиотеке, т.к. его вид 
 // отличается в Си и Делфи
 // Функционал метода почти тот же что и у PatcherInstance::WriteCodePatch
 // (см. описание этого метода)
 // то есть метод пишет по адресу address, последовательность байт,
 // определенную аргументами format и ...,
 // НО! НЕ создает экземпляр класса Patch, со всеми вытекающими (т.е. не позволяя отменить правку, получить доступ к правке из другого мода и т.д.)
 // ВНИМАНИЕ!
 // Используйте этот метод только для динамического создания блоков
 // кода, т.е. пишите этим методом только в свою память, 
 // а в код модифицируемой программы только с помощью
 // PatcherInstance::WriteCodePatch
 //
 inline _ptr_ WriteComplexData(_ptr_ address, char const* format, ...)
 {
  return WriteComplexDataVA(address, format, (_dword_*)((_ptr_)&format + 4));
 }



inline HiHook* GetFirstHiHookAt(_ptr_ address)
{
	Patch* p = GetFirstPatchAt(address);
	while (true)
	{
		if (p == 0) return 0;
		if (p->GetType() == HIHOOK_) return (HiHook*)p;
		p = p->GetAppliedAfter();
	}
}

inline HiHook* GetLastHiHookAt(_ptr_ address)
{
	Patch* p = GetLastPatchAt(address);
	while (true)
	{
		if (p == 0) return 0;
		if (p->GetType() == HIHOOK_) return (HiHook*)p;
		p = p->GetAppliedBefore();
	}
}


};





// восстанавливаем выравнивание членов структур и классов
#pragma pack(pop)

//////////////////////////////////////////////////////////////////

//функция GetPatcher
//загружает библиотеку и, с помощью вызова единственной экспортируемой 
//функции _GetPatcherX86@0, возвращает указатель на объект Patcher,
//посредством которого доступен весь функционал библиотеки patcher_x86.dll
//возвращает NULL при неудаче
//функцию вызывать 1 раз, что очевидно из ее определения
#include <windows.h>
inline Patcher* GetPatcher()
{
static int calls_count = 0;
 if (calls_count > 0) return NULL;
 calls_count++;
 HMODULE pl = LoadLibraryA("patcher_x86.dll");
 if (pl)
 {
  FARPROC f = GetProcAddress(pl, "_GetPatcherX86@0");
  if (f) return CALL_0(Patcher*, __stdcall, f);
 }
 return NULL;
}
