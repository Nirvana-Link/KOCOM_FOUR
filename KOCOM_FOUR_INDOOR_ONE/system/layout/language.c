#include"language.h"


const char *multi_lingual[STR_TOTAL][LANGUAGE_TOTAL] = 
{
    {"0","0","0","0","0"},
    {"1","1","1","1","1"},
    {"2","2","2","2","2"},
    {"3","3","3","3","3"},
    {"4","4","4","4","4"},
    {"DOOR CAMERA","현관 카메라","CAM","كاميرا","Camera","Kамера"},
    {"INTERCOM","내선 통화","Intercomunicador","اللانترفون","Liên lạc nội bộ","Интерком"},
    {"NEW CAPTURE","방문자 확인","Nueva captura","صورة جديدة","Ghi hình mới","Новая запись"},
    {"DO NOT DISTURB","방해금지 모드","No molestar","عدم ازعاج","Không làm phiền","Не беспокоить"},
    {"ABSENT MODE","외출 모드","Modo ausente","وضع الغياب","Chế độ vắng mặt","Режим отсутствия"},
    {"Setting","세팅","Ajuste","إعدادات","Cài đặt","Настройка"},
    {"MON","월요일","LUN","الاثنين","Thứ 2","Понедельник"},
    {"TUE","화요일","MAR","الثلاثاء","Thứ 3","Вторник"},
    {"WED","수요일","MIE","الأربعاء","Thứ 4","Среда"},
    {"THU","목요일","JUE","الخميس","Thứ 5","Четверг"},
    {"FRI","금요일","VIE","الجمعة","Thứ 6","Пятница"},
    {"SAT","토요일","SAB","السبت","Thứ 7","Суббота"},
    {"SUN","일요일","DOM","الأحد","Chủ nhật","Воскресенье"},
    {"ALL","전체","TODOS","الكل","Tất cả","Все"},
    {"Device ID","디바이스 ID","Identificación del dispositivo","معرف الجهاز ","ID thiết bị","ID устройства"},
    {"Device","디바이스","Dispositivo","الجهاز","Thiết bị","Устройство"},
    {"Device number conflict","디바이스 ID 중복","Conflicto de número de dispositivo","تعارض رقم الجهاز","Xung đột số thiết bị","Ошибка номера устройства"},
    {"DEVICE 1","디바이스 1","Dispositivo 1","الجهاز 1","Thiết bị 1","Устройство 1"},
    {"DEVICE 2","디바이스 2","Dispositivo 2","الجهاز 2","Thiết bị 2","Устройство 2"},
    {"DEVICE 3","디바이스 3","Dispositivo 3","الجهاز 3","Thiết bị 3","Устройство 3"},
    {"DEVICE 4","디바이스 4","Dispositivo 4","الجهاز 4","Thiết bị 4","Устройство 4"},
    {"PM","오후","PM","المساء","Chiều","PM"},
    {"AM","오전","AM","الصباح","Sáng","AM"},
    {"Door Opened","문열림","Puerta abierta","الباب مفتوح","Cửa đã mở","Дверь открыта"},
    {"RESET","초기화","REINICIAR","إعادة ضبط ","Nghỉ ngơi","Сброс"},
    {"Yes","네","Sí","نعم","Có","Да"},
    {"No","아니오","No","لا","Không","Нет"},
    {"Do you want to reset?","리셋할까요?","Quiere resetear","هل ترغب بإعادة الضبط","Bạn có muốn cài đặt lại ?","Вы хотите сбросить настройки?"},
    {"General","일반 설정","General","عام","Tổng quan","Общие"},
    {"Sound","사운드","Sonido","الصوت ","Âm thanh","Звук"},
    {"Connect","WiFi 연결","Conectar","اتصال","Kết nối","Соединять"},
    {"Mode","모드 설정","Modo","الوضع","Chế độ","Режим"},
    {"Other","기타 설정","Otro","أخرى","Khác","Другое"},
    {"Language","언어","Idioma","اللغة ","Ngôn ngữ","Язык"},
    {"English","English","English","English","English","English"},
    {"한글","한글","한글","한글","한글","한글"},
    {"Español","Español","Español","Español","Español","Español"},
    {"русский","русский","русский","русский","русский","русский"},
    {"عربى","عربى","عربى","عربى","عربى","عربى"},
    {"Tiếng Việt","Tiếng Việt","Tiếng Việt","Tiếng Việt","Tiếng Việt","Tiếng Việt"},
    {"Time","시간","Hora","الوقت","Thời gian","Время"},
    {"Date","날짜","Fecha","التاريخ ","Ngày","Дата"},
    {"Interphone","스마트폰","Interfono","انترفون(جهاز اتصال)","Interphone","Телефон на линии связи"},
    {"Calling Volume","벨소리 음량","Volumen de llamada","مستوى الاتصال ","Âm lượng cuộc gọi","Громкость вызова"},
    {"Talk Volume","통화 음량","Volumen de conversación","مستوى صوت التحدث ","Âm lượng giọng nói","Громкость разговора"},
    {"Calling Melody","벨소리","Melodía","نغمة الاتصال ","Giai điệu chuông gọi","Выбор мелодии"},
    {"Speaker Sensitivity","통화 감도","Sensibilidad del altavoz","حساسية مكبر الصوت","Độ nhạy của loa","Настройка динамика"},
    {"MIC Sensitivity","마이크 감도","Sensibilidad MIC","حساسية المايك ","Độ nhạy micro","Настройка микрофона"},
    {"Always On Display","상시 시계화면","Siempre encendido","دائما تشغيل ","Luôn mở","Всегда включено"},
    {"Auto Image Capture","화면 자동캡쳐","Captura automática","لقطة شاشة تلقائية ","Chụp hình tự động","Автоматическая запись"},
    {"Network Time","네트워크 시간","Hora de la red","وقت الشبكة","Thời gian mạng","Сетевое время"},
    {"Auto Recode video","동영상 자동 저장","Auto Recodificar video","تسجيل فيديو تلقائي","Tự động ghi video","Автоматическая запись видео"},
    {"Screen Brightness","화면 밝기","Brillo de la pantalla","سطوع الشاشة","Độ sáng màn hình","Яркость экрана"},
    {"Absent Mode Delay","외출설정 시간","Retardo de modo ausente","تأخير وضع الغياب","Độ trễ chế độ vắng mặt","Задержка режима"},
    {"Monitoring Time","모니터링 시간","Tiempo de Monitoreo","وقت المراقبة ","Thời gian giám sát","Время просмотра"},
    {"Door Open Time","도어개폐 시간","Tiempo de puerta abierta","وقت الباب ","Thời gian mở cửa","Время открытия двери"},
    {"Modify Password","비밀번호 수정","Modificar contraseña","تعديل  كلمة السر","Đổi  mât khẩu","Изменить пароль"},
    {"Old","현재 비밀번호","Anterior","قديم ","Cũ","Старый пароль"},
    {"New","신규 비밀번호","Nuevo","جديد","Mới","Новый пароль"},
    {"Setting Device","디바이스 세팅","Configuración del dispositivo","إعدادات الجهاز","Cài đặt thiết bị","Настройка устройства"},
    {"Sensor 1","센서 1","Sensor 1","حساس 1","Cảm biến 1","Датчик 1"},
    {"Sensor 2","센서 2","Sensor 2","حساس 2","Cảm biến 2","Датчик 2"},
    // {"CAM 1","카메라 1","CAM 1","كاميرا 1","Camera 1","Кamepa 1"},
    {"Camera1","카메라 1","CAM 1","كاميرا 1","Camera 1","Камера 1"},
    // {"CAM 2","카메라 2","CAM 2","كاميرا 2","Camera 2","Кamepa 2"},
    {"Camera2","카메라 2","CAM 2","كاميرا 2","Camera 2","Камера 2"},
    {"CCTV 1","CCTV 1","CCTV 1","كاميرا 1","CCTV 1","CCTV 1"},
    {"CCTV 2","CCTV 2","CCTV 2","كاميرا 2","CCTV 2","CCTV 2"},
    {"Brand","CCTV 브랜드","Marca","العلامة التجارية","Nhãn hàng","Модель"},
    {"IP","IP","IP","عنوان IP","IP","IP межсетевой протокол"},
    {"Account","계정","Cuenta","الحساب","Tài khoản","Учётная запись"},
    {"Password","비밀번호","Contraseña","كلمة السر","Mật khẩu","Пароль"},
    {"Add manually","WiFi 수동설정","Agregar manualmente","اضافة يدوية","Thêm vào thủ công","Добавить вручную"},
    {"Wifi name","WiFi","Nombre Wifi","اسم الشبكة اللاسلكية","Tên wifi","Имя Wifi сети"},
    // {"HIK","HIK","HIK","HIK","HIK"},
    // {"DAH","DAH","DAH","DAH","DAH"},*
    {"Camera Conflict","카메라 충돌","Conflicto CAM","صراع الكاميرا","xung đột camera","Конфликт камеры"},
    {"DISCONNECTED", "연결 끊김", "desconectado", "انقطع الاتصال", "ngắt kết nối","Отключено"},
    {"NO SIGNAL","신호 없음","SIN SEÑAL","لا توجد تغطية","KHÔNG CÓ TÍN HIỆU","НЕТ СИГНАЛА"},
    {"Camera Busy","카메라 사용 중","Cámara ocupada","الكاميرا مشغولة","Máy ảnh bận","Камера занята"},
    {"NO DEVICE","장치 없음","SIN DISPOSITIVO","لا يوجد جهاز","KHÔNG CÓ THIẾT BỊ","НЕТ УСТРОЙСТВА"},
    {"Start","시작","COMIENZO","تشغيل ","Bắt đầu","СТАРТ"},
    {"Stop","정지","DETENER","ايقاف ","Ngưng","СТОП"},
    {"Active Now","즉시 실행","Activo ahora","الآن مفعل ","Đang hoạt động","Активен"},
    {"System Information","시스템 정보","Información del sistema","معلومات النظام ","Thông tin hệ thống","Системная информация"},
    {"Check  Updates","업데이트 확인","Revisa actualizaciones","تفقد التحديث","Kiểm tra cập nhật","Проверить обновления"},
    {"outdoor1","outdoor1","outdoor1","outdoor1","outdoor1","outdoor1"},
    {"outdoor2","outdoor2","outdoor2","outdoor2","outdoor2","outdoor2"},
    {"Quit","나가기","Renunciar","استقال","từ bỏ","Покидать"},
    {"NOTICE","※ 주의","AVISO","تنويه","LƯU Ý","УВЕДОМЛЕНИЕ"},
    {"Please check the network connection,","네트워크 연결과 스마트폰 연동을 확인하십시오.","Por favor, compruebe la conexión de red,","يرجى التحقق من اتصال الشبكة ،","Vui lòng kiểm tra kết nối mạng,","Пожалуйста, проверьте подключение к сети,"},
    {"and make sure the device is bound to the phone."," ","y asegúrese de que el dispositivo esté vinculado al teléfono.","وتأكد من أن الجهاز مرتبط بالهاتف.","và đảm bảo thiết bị được kết nối với điện thoại.","и убедитесь, что устройство привязано к телефону."},
    {"Modified successfully!","변경 완료","¡Modificado con éxito!","تم التعديل بنجاح","Đã sửa đổi thành công !","Успешно сохранено!"},
    {"Delete All Photo?","이미지 전체 삭제","¡Eliminar todas las fotos!","حذف كل الصور ","Xoá tất cả hình ảnh","Удалить все фото"},
    {"Delete This Photo?","현재 이미지 삭제","¡Borra esta foto!","حذف هذه الصورة ","Xoá ảnh này","Удалить фото"},
    {"Delete All?","전체 삭제","Eliminar todos","حذف الكل ","Xoá tất cả","Удалить всё"},
    {"Authentication Failed!","인증 실패","¡Autenticación fallida!","تم فشل المطابقة ","Quá trình xác thực thất bại !","Ошибка аутентификации!"},
    {"Resetting, please wait.","재설정 중입니다. 잠시만 기다려 주십시오.","Restableciendo, por favor espere.","إعادة التعيين ، يرجى الانتظار.","Đang đặt lại, vui lòng đợi.","Сброс, подождите."},
    {"Manual Download","제품 설명서","Descargar manual","اليدوي تحميل","Tải về bản hướng dẫn sử dụng cụ","Руководство по продукту"},
    {"Auto Set","자동 설정 시간","Ajuste automático","ضبط الوقت التلقائي","Thiết lập tự động","Авто"},
    {"Phone","스마트폰","Teléfono","هاتف","Điện thoại","Телефон"},
    {"Only support one device and APP link","하나의 장치와 APP 링크만 지원","Solo admite un dispositivo y un enlace de aplicación","دعم جهاز واحد فقط ورابط التطبيق","Chỉ hỗ trợ một thiết bị và liên kết APP","Поддерживает только одно устройство и ссылку на приложение"},
    {"There are duplicate devices or connections.","중복된 장치 또는 연결이 있습니다.","Hay dispositivos o conexiones duplicados.","هناك أجهزة أو اتصالات مكررة.","Có các thiết bị hoặc kết nối trùng lặp.","Имеются повторяющиеся устройства или подключения."},
    {"Disconnecting from the APP requires a reboot, now reboot.","스마트폰 연동 해제를 원하시면, 재부팅하십시오.","Desconectarse de la aplicación requiere un reinicio, ahora reinicie.","يتطلب قطع الاتصال من التطبيق إعادة التشغيل ، والآن أعد التشغيل.","Ngắt kết nối khỏi APP yêu cầu khởi động lại, bây giờ hãy khởi động lại.","Отключение от приложения требует перезагрузки, теперь перезагрузка."},
    {"Device Busy","장치 사용 중","Dispositivo ocupado","الجهاز مشغول","Thiết bị bận","Устройство занято"},
    {"Internal Call","내선 통화","Llamada interna","اتصال داخلي","Gọi nội bộ","Внутренний вызов"},
    {"DUPLICATE ID", "ID 중복", "ID duplicado", "معرف مكرر", "ID trùng lặp","Ошибка номера устройства"},
    {"Approval","승인","Aprobación","موافقة","Sự chấp thuận","Одобрение"},
    {"Back","취소","Atrás","خلف","Trở lại","Назад"},
};
void* btn_str(enum btn_string_id str_id)
{
    return (void*)multi_lingual[str_id];
}

const char* text_str(enum btn_string_id str_id){

    return  (const char*)multi_lingual[str_id][language_get()];
}

#define STANDARD_SIZE1 22 //23
#define STANDARD_SIZE2 20 //22

const int standard_size[language_total] = {
        STANDARD_SIZE1,      // language_english
        STANDARD_SIZE1,      // language_korean
        STANDARD_SIZE2,      // language_spanish
        STANDARD_SIZE1,      // language_arabic
        STANDARD_SIZE2,      // language_vietnamese
        STANDARD_SIZE2,      // language_russian
};

int font_size(enum font_size type)
{
    switch (type)
    {
    case HOME_BUTTON/* constant-expression */:
        return standard_size[language_get()] - 4;

    case HOME_DATE_STR:
        return standard_size[language_get()] + 8;

    case HOME_DATE:
        return standard_size[language_get()];

    case INTERCOM_BUTTON:
        return standard_size[language_get()];

    case HEADLINE:
        return standard_size[language_get()] + 14;

    case HOUSE_DEVICE:
        return standard_size[language_get()];
    
    case CAM_DOOR:
    return standard_size[language_get()] - 2;

    case CAM_DOOR_SET:
    return standard_size[language_get()];

    case SET_OPTION:
    return standard_size[language_get()] + 8;

    case SET_OPTION_INFO:
    return standard_size[language_get()];

    case SET_RESET_OPTION:
    return standard_size[language_get()];

    case CAPTURE_BUTTON:
    return standard_size[language_get()] + 6;

    case CAPTURE_INFO:
    return standard_size[language_get()] + 4;

    case ALARM_INFO:
    return standard_size[language_get()] + 4;
    
    case ABSENT_INFO:
    return standard_size[language_get()] + 2;

    case ABSENT_BUTTON:
    return standard_size[language_get()];
    default:
        break;
    }
    return standard_size[language_get()];
}

