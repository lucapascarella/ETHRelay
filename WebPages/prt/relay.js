/*
 * Copyright (C) 2016 LP Systems
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
 * in compliance with the License. You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License
 * is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
 * or implied. See the License for the specific language governing permissions and limitations under
 * the License.
 * 
 * Author: Luca Pascarella www.lucapascarella.it
 */

function updateSelectWithOptions() {
    for (var i = 1; i <= 4; i++) {
        for (var j = 0; j < 24; j++) {
            $('#on1h' + i + ',#off1h' + i + ',#on2h' + i + ',#off2h' + i).append($('<option>', {
                value: j,
                text: j
            }));
        }
    }
    for (var i = 1; i <= 4; i++) {
        for (var j = 0; j < 60; j++) {
            $('#on1m' + i + ', #off1m' + i + ',#on2m' + i + ', #off2m' + i).append($('<option>', {
                value: j,
                text: j
            }));
        }
    }
}

function relay(tag) {

    var id = tag.id;
    var num = id.substring(2, id.lenght);

    var obj = {};
    obj['relay'] = num;
    obj['act'] = $(tag).is(":checked") ? "1" : "0";

    $.ajax({
        type: "GET",
        url: "/prt/relay.cgi",
        data: obj,
        cache: false,
        timeout: 20000,
        success: function (data) {
            //showAccept(tag, 3000);
            updateRelay(false, true);
        },
        error: function (jqXHR, textStatus, errorThrown) {
            showExclamation(tag, 8000);
            showMessageInBox("#msg", "An error has occurred when updating.<br/>Error status: " + textStatus + "<br/>Error thrown: " + errorThrown, "fail", 10000);
        },
        complete: function () {
            //hiddenLoading();
        }
    });
}

function startup(tag) {

    var id = tag.id;
    var num = id.substring(2, id.lenght);

    var obj = {};
    obj['relay'] = num;
    obj['st'] = $(tag).is(":checked") ? "1" : "0";

    $.ajax({
        type: "GET",
        url: "/prt/relay.cgi",
        data: obj,
        cache: false,
        timeout: 20000,
        success: function (data) {
            //showAccept(tag, 3000);
            updateRelay(false, true);
        },
        error: function (jqXHR, textStatus, errorThrown) {
            showExclamation(tag, 8000);
            showMessageInBox("#msg", "An error has occurred when updating.<br/>Error status: " + textStatus + "<br/>Error thrown: " + errorThrown, "fail", 10000);
        },
        complete: function () {
            //hiddenLoading();
        }
    });
}

function reset(tag) {
    var id = tag.id;
    var num = id.substring(2, id.lenght);
    $("#on1h" + num + ",#on2h" + num).val("0").change();
    $("#on1m" + num + ",#on1m" + num).val("0").change();
    $("#off1h" + num + ",#off1h" + num).val("0").change();
    $("#off1m" + num + ",#off1m" + num).val("0").change();
    $("#on2h" + num + ",#on2h" + num).val("0").change();
    $("#on2m" + num + ",#on2m" + num).val("0").change();
    $("#off2h" + num + ",#off2h" + num).val("0").change();
    $("#off2m" + num + ",#off2m" + num).val("0").change();
    save(tag);
}

function checkTime(on1, off1, on2, off2) {
    if (on1 === off1 || on2 === off2)
        return true;
    if (on1 > off1) {
        // Inverse
        if (on2 < off1 || off2 > on1 || on2 > off2) {
            // Not allowed
            return false;
        }
    } else if (on2 > off2) {
        // Inverse
        if (on1 < off2 || off1 > on2) {
            // Not allowed
            return false;
        }
    } else {
        // Normal
        if (on1 < on2 && on2 < off1 || on2 < on1 && on1 < off2) {
            // Not allowed
            return false;
        }
    }
    return true;
}

function save(tag) {

    var id = tag.id;
    var num = id.substring(2, id.lenght);

    showLoading(tag);
    $(tag).prop("disabled", true);

    var obj = {};
    obj['relay'] = num;
    var on1h = parseInt($("#on1h" + num).val(), 10);
    var on1m = parseInt($("#on1m" + num).val(), 10);
    var off1h = parseInt($("#off1h" + num).val(), 10);
    var off1m = parseInt($("#off1m" + num).val(), 10);
    var on2h = parseInt($("#on2h" + num).val(), 10);
    var on2m = parseInt($("#on2m" + num).val(), 10);
    var off2h = parseInt($("#off2h" + num).val(), 10);
    var off2m = parseInt($("#off2m" + num).val(), 10);

    if (checkTime(on1h * 60 + on1m, off1h * 60 + off1m, on2h * 60 + on2m, off2h * 60 + off2m)) {
        obj['on1h'] = on1h;
        obj['on1m'] = on1m;
        obj['off1h'] = off1h;
        obj['off1m'] = off1m;
        obj['on2h'] = on2h;
        obj['on2m'] = on2m;
        obj['off2h'] = off2h;
        obj['off2m'] = off2m;

        $.ajax({
            type: "GET",
            url: "/prt/relay.cgi",
            data: obj,
            cache: false,
            timeout: 20000,
            success: function (data) {
                showAccept(tag, 3000);
                updateRelay(false, true);
            },
            error: function (jqXHR, textStatus, errorThrown) {
                showExclamation(tag, 8000);
                showMessageInBox("#msg", "An error has occurred when updating.<br/>Error status: " + textStatus + "<br/>Error thrown: " + errorThrown, "fail", 10000);
            },
            complete: function () {
                hiddenLoading();
            }
        });
    } else {
        showMessageInBox("#msg", "Verify Relay " + num + " interval time and try again.", "fail", 10000);
    }
}


function updateRelay(update, relay) {
    $.ajax({
        type: "GET",
        url: "/prt/relay.xml",
        cache: false,
        timeout: 6000,
        success: function (data) {
            $xml = $(data);
            for (var i = 1; i <= 4; i++) {
                $("#rb" + i).prop('checked', $xml.find("r r" + i).text() === "0" ? false : true);
                $("#st" + i).prop('checked', $xml.find("r st" + i).text() === "0" ? false : true);

                var on1h = $xml.find("r on1h" + i).text();
                var on1m = $xml.find("r on1m" + i).text();
                var off1h = $xml.find("r off1h" + i).text();
                var off1m = $xml.find("r off1m" + i).text();
                var on2h = $xml.find("r on2h" + i).text();
                var on2m = $xml.find("r on2m" + i).text();
                var off2h = $xml.find("r off2h" + i).text();
                var off2m = $xml.find("r off2m" + i).text();
                if (relay) {
                    $("#on1h" + i).val(on1h);
                    $("#on1m" + i).val(on1m);
                    $("#off1h" + i).val(off1h);
                    $("#off1m" + i).val(off1m);
                    $("#on2h" + i).val(on2h);
                    $("#on2m" + i).val(on2m);
                    $("#off2h" + i).val(off2h);
                    $("#off2m" + i).val(off2m);
                }
                if (on1h === off1h && on1m === off1m && on2h === off2h && on2m === off2m) {
                    $("#rb" + i).prop('disabled', false);
                    $("#rb" + i).next().find("span").removeClass("onoff-disabled");
                } else {
                    $("#rb" + i).prop('disabled', true);
                    $("#rb" + i).next().find("span").addClass("onoff-disabled");
                }
            }
        },
        error: function (jqXHR, textStatus, errorThrown) {
        },
        complete: function () {
            $("#rs1,#rs2,#rs3,#rs4,#rr1,#rr2,#rr3,#rr4").prop("disabled", false);
            if (update) {
                setTimeout("updateRelay(true, false);", 1500);
            }
        }
    });
}

