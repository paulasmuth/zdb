
dateUtil = this.dateUtil || {};
dateUtil.kSecondsPerMinute = 60;
dateUtil.kSecondsPerHour = 3600;
dateUtil.kSecondsPerDay = 86400;
dateUtil.kMillisPerSecond = 1000;
dateUtil.kMillisPerMinute = dateUtil.kMillisPerSecond * dateUtil.kSecondsPerMinute;
dateUtil.kMillisPerHour = dateUtil.kSecondsPerHour * dateUtil.kMillisPerSecond;
dateUtil.kMillisPerDay = dateUtil.kSecondsPerDay * dateUtil.kMillisPerSecond;
dateUtil.kMillisPerWeek = dateUtil.kMillisPerDay * 7;

dateUtil.toUTC = function(timestamp) {
  var date = new Date(timestamp);
  return timestamp - (date.getTimezoneOffset() * dateUtil.kMillisPerMinute);
}

dateUtil.toLocal = function(timestamp) {
  var date = new Date(timestamp);
  return timestamp + (date.getTimezoneOffset() * dateUtil.kMillisPerMinute);
}

/**
  * Format to a date time string yyyy-mm-dd hh:mm
  * @param timestamp to format
  * @param timezone determines the timezone (must be utc or local, defaults to local)
**/
dateUtil.formatDateTime = function(timestamp, timezone) {
  function appendLeadingZero(num) {
    if (num < 10) {
      return "0" + num;
    }

    return "" + num;
  }

  var d;
  if (timezone == 'utc') {
    d = new Date(dateUtil.toLocal(timestamp));
  } else {
    d = new Date(timestamp);
  }
  return [
      d.getFullYear(), "-",
      appendLeadingZero(d.getMonth() + 1), "-",
      appendLeadingZero(d.getDate()), " ",
      appendLeadingZero(d.getHours()), ":",
      appendLeadingZero(d.getMinutes())].join("");
}

