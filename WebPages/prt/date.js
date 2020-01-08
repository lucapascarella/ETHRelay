




//function enableDSTCheck() {
//    if (role === 0x80) {
//        $("#dste").prop("checked", !$("#dste").is(":checked"));
//        enableDST();
//    }
//}

function enableDST() {
    var cond = !$("#dste").is(":checked");
    $("#dst").prop("disabled", cond);
}

//function enableNTPCheck() {
//    if (role === 0x80) {
//        $("#ntpe").prop("checked", !$("#ntpe").is(":checked"));
//        enableNTP();
//    }
//}

function enableNTP() {
    var cond = !$("#ntpe").is(":checked");
    $("#ntp").prop("disabled", cond);
}



function disableSaveDataTime(cond) {
    $("#saveDateTime").prop("disabled", cond);
}

function ajaxSaveDateTime(tag) {

    disableSaveDataTime(true);

    var obj = {};
    // Manual set Date and Time
    obj['man'] = '0';
    obj['hh'] = $("#hour").val();
    obj['mm'] = $("#minute").val();
    obj['ss'] = $("#second").val();

    obj['dd'] = $("#day").val();
    obj['MM'] = $("#month").val();
    obj['yy'] = $("#year").val();

    if (obj['hh'] === "" || obj['hh'] < 0 || obj['hh'] > 23) {
        alert("There is an error on hours. It must be a number between 00 and 23");
        $("#hour").val("");
        return;
    }
    if (obj['mm'] === "" || obj['mm'] < 0 || obj['mm'] > 59) {
        alert("There is an error on minutes field. It must be a number between 00 and 59");
        $("#minute").val("");
        return;
    }
    if (obj['ss'] === "" || obj['ss'] < 0 || obj['ss'] > 59) {
        alert("There is an error on seconds. It must be a number between 00 and 59");
        $("#second").val("");
        return;
    }
    if (obj['dd'] === "" || obj['dd'] < 1 || obj['dd'] > 31) {
        alert("There is an error on month day. It must be a number between 01 and 31");
        $("#day").val("");
        return;
    }
    if (obj['MM'] === "" || obj['MM'] < 1 || obj['MM'] > 12) {
        alert("There is an error on month. It must be a number between 01 and 12");
        $("#month").val("");
        return;
    }
    if (obj['yy'] === "" || obj['yy'] < 2000 || obj['yy'] > 2099) {
        alert("There is an error on year. It must be a number between 2020 and 2099");
        $("#year").val("");
        return;
    }

    showLoading(tag);
    $.ajax({
        type: "GET",
        url: '/prt/date.cgi',
        cache: false,
        data: obj,
        timeout: 10000,
        success: function (data) {
            showAccept(tag, 5000);
            showMessageInBox("#msg", "The new settings are saved correctly but they will be available at the next restart", "ok", 15000);
        },
        error: function (jqXHR, textStatus, errorThrown) {
            showExclamation(tag, 8000);
            showMessageInBox("#msg", "An error has occurred.<br/>Error status: " + textStatus + "<br/>Error thrown: " + errorThrown, "fail", 20000);
        },
        complete: function () {
            hiddenLoading();
        }
    });
}


function disableSaveDate(cond) {
    $("#saveDate").prop("disabled", cond);
//	$("#applyDate").prop("disabled", cond);
    $("#saveAndApplyDate").prop("disabled", cond);
    $("#resetDate").prop("disabled", cond);
}

// Save parameters functions
function saveDate(tag) {
    var obj = {};
    obj['save'] = "1";
    ajaxSaveDate(obj, tag);
}

function saveAndApplyDate(tag) {
    var obj = {};
    obj['save'] = "1";
    obj['appl'] = "1";
    ajaxSaveDate(obj, tag);
}

function ajaxSaveDate(request, tag) {

    disableSaveDate(true);
    showLoading(tag);
    var obj = {};

    obj['gmt'] = $("#gmt").val();
    obj['dste'] = $("#dste").is(":checked") ? "1" : "0";
    obj['dst'] = $("#dst").val();
    obj['ntpe'] = $("#ntpe").is(":checked") ? "1" : "0";
    obj['ntp'] = $("#ntp").val();

    $.ajax({
        type: "GET",
        url: "/prt/date.cgi",
        cache: false,
        data: obj,
        timeout: 10000,
        success: function (data) {
            showAccept(tag, 5000);
            showMessageInBox("#msg", "The new settings were saved correctly.", "ok", 15000);
        },
        error: function (jqXHR, textStatus, errorThrown) {
            showExclamation(tag, 8000);
            showMessageInBox("#msg", "An error has occurred.<br/>Error status: " + textStatus + "<br/>Error thrown: " + errorThrown, "fail", 20000);
        },
        complete: function () {
            hiddenLoading();
        }
    });
}
/*
 * Update the current time and periodically resync the current time with server 
 */
function getDate() {

    var time_val = $("#time").text();
    var time = parseInt(time_val.substring(time_val.lastIndexOf(':') + 1, time_val.lenght));

    if ((time % 10) === 9) {
        $.ajax({
            type: "GET",
            url: "/prt/date.xml",
            cache: false,
            timeout: 10000,
            success: function (data) {
                $xml = $(data);
                $("#time").text($xml.find("r t").text());
                $("#date").text($xml.find("r d").text());
                console.log("Time re-synchronized with server");
            },
            error: function (jqXHR, textStatus, errorThrown) {
                console.log("Server sync is lost");
            }
        });
    } else {
        if (++time < 10)
            $("#time").text(time_val.substring(0, time_val.lastIndexOf(':')) + ":0" + time);
        else
            $("#time").text(time_val.substring(0, time_val.lastIndexOf(':')) + ":" + time);
    }
    setTimeout("getDate();", 1000);
}

function forceUpdate(tag) {

    $("#" + tag.id).prop("disabled", true);
    showLoading(tag);
    var obj = {};
    obj['force'] = '1';

    $.ajax({
        type: "GET",
        url: "/prt/date.cgi",
        cache: false,
        data: obj,
        timeout: 10000,
        success: function (data) {
            showAccept(tag, 5000);
            showMessageInBox("#msg", "Request sent for new NTP update", "ok", 25000);
        },
        error: function (jqXHR, textStatus, errorThrown) {
            showExclamation(tag, 8000);
            showMessageInBox("#msg", "An error has occurred.<br/>Error status: " + textStatus + "<br/>Error thrown: " + errorThrown, "fail", 45000);
        },
        complete: function () {
            hiddenLoading();
            $("#" + tag.id).prop("disabled", false);
        }
    });

}