function doGet(){ 
    return ContentService.createTextOutput(GetEvents());
  }
  
  // Replace MySchedules with your calendar name
  function GetEvents(){
    var _calendarName = 'MySchedules'
    var Cal = CalendarApp.getCalendarsByName(_calendarName)[0];
    var Now = new Date();
    var Later = new Date();
    Later.setSeconds(Now.getSeconds() + 1); 
    Logger.log(Later);
    var events = Cal.getEvents(Now, Later); 
    str = "";
    for (var i = 0; i < events.length; i++){
      str += events[i].getTitle() + ';' ; 
    }
    str = str.substring(0, str.length - 1); 
    return str;
  }