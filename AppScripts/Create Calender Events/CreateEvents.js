function doGet(eve){
    var title = eve.parameter.title;
    Logger.log(eve); 
    return ContentService.createTextOutput(CreateEvent(title));
  }
  
  // Replace 'MySchedules' with your calendar name
  function CreateEvent(eve){
    var _calendarName = 'MySchedules'  // calender name
    var MILLIS_PER_HOUR = 1000 * 60 * 15 ;
    var now = new Date();
    var from = new Date(now.getTime()-60000);
    var to = new Date(now.getTime() + 1 * MILLIS_PER_HOUR);
    var vCalendar = CalendarApp.getCalendarsByName(_calendarName)[0];
     vCalendar.createEvent(eve,
                          from,
                          to) 
  return true;
  }