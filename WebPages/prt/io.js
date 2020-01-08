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

function createHTML() {
    for (var i = 1; i <= 8; i++) {
        var str = '<tr><td>Input/Output ' + i + ':</td><td><div class="onoff"><input type="checkbox" class="onoff-checkbox" id="io' + i + '" onclick="inout(this);"/><label class="onoff-label" for="io' + i + '" title="Define input or output IO direction"><span class="onoff-inner onoff-inout"></span><span class="onoff-switch"></span></label></div></td><td><div class="onoff"><input type="checkbox" class="onoff-checkbox" id="out' + i + '" onclick="output(this);"/><label class="onoff-label" for="out' + i + '" title="If the IO is in output mode it allows to change the status, otherwise shows the current status"><span class="onoff-inner onoff-onoff"></span><span class="onoff-switch"></span></label></div></td><td><div class="onoff"><input type="checkbox" class="onoff-checkbox" id="de' + i + '" onclick="def(this);"/><label class="onoff-label" for="de' + i + '" title="If the IO is in output mode it allows to change the default startup status"><span class="onoff-inner onoff-onoff"></span><span class="onoff-switch"></span></label></div></td></tr>';
        $("#table1").append(str);

        str = '<tr><td>Input ' + i + ':</td><td><input type="text" id="add' + i + '" maxlength="63" title="IPV4 or URL address"/></td><td><input type="text" id="user' + i + '" maxlength="15" title="Username of the remote board"/></td><td><input type="password" id="pass' + i + '" maxlength="15" title="Password of the remote board"/></td><td><select id="sel' + i + '" onchange="enable(this)"><optgroup label="None"><option value="0">Disabled</option></optgroup><optgroup label="Digital output"><option value="1">Output 1</option><option value="2">Output 2</option><option value="3">Output 3</option><option value="4">Output 4</option><option value="5">Output 5</option><option value="6">Output 6</option><option value="7">Output 7</option><option value="8">Output 8</option></optgroup><optgroup label="Relay"><option value="9">Relay 1</option><option value="10">Relay 2</option><option value="11">Relay 3</option><option value="12">Relay 4</option></optgroup></select></td><td><input type="button" id="save' + i + '" value="Save" onclick="saveIO(this);"/></td></tr>';
        $("#table2").append(str);

        str = '<tr id="out' + i + '"><td>Output ' + i + ':</td><td><select id="on1h' + i + '"></select><select id="on1m' + i + '"></select></td><td><select id="off1h' + i + '"></select><select id="off1m' + i + '"></select></td><td><select id="on2h' + i + '"></select><select id="on2m' + i + '"></select></td><td><select id="off2h' + i + '"></select><select id="off2m' + i + '"></select></td><td><input type="button" value="Save" id="os' + i + '" onclick="saveTimer(this);" title=""/><input type="button" value="Reset" id="or' + i + '" onclick="resetTimer(this);" title=""/></td></tr>';
        $("#table3").append(str);
    }
}

function output(tag) {
    var id = tag.id;
    var num = id.substring(3, id.lenght);

    var obj = {};
    obj['io'] = num;
    obj['act'] = $(tag).is(":checked") ? "1" : "0";

    $.ajax({
        type: "GET",
        url: "/prt/io.cgi",
        data: obj,
        cache: false,
        timeout: 20000,
        success: function (data) {
            //showAccept(tag, 3000);
            updateIO(false, true);
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

function inout(tag) {
    var id = tag.id;
    var num = id.substring(2, id.lenght);

    var obj = {};
    obj['io'] = num;
    obj['dir'] = $(tag).is(":checked") ? "1" : "0";

    $.ajax({
        type: "GET",
        url: "/prt/io.cgi",
        data: obj,
        cache: false,
        timeout: 20000,
        success: function (data) {
            //showAccept(tag, 3000);
            updateIO(false, true);
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

function def(tag) {
    var id = tag.id;
    var num = id.substring(2, id.lenght);

    var obj = {};
    obj['io'] = num;
    obj['st'] = $(tag).is(":checked") ? "1" : "0";

    $.ajax({
        type: "GET",
        url: "/prt/io.cgi",
        data: obj,
        cache: false,
        timeout: 20000,
        success: function (data) {
            //showAccept(tag, 3000);
            updateIO(false, true);
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

function updateIO(update, io) {
    $.ajax({
        type: "GET",
        url: "/prt/io.xml",
        cache: false,
        timeout: 6000,
        success: function (data) {
            $xml = $(data);
            for (var i = 1; i <= 8; i++) {
                //$("#in" + i).prop('checked', $xml.find("r in" + i).text() === "0" ? false : true);
                if ($xml.find("r io" + i).text() === "0") {
                    $("#io" + i).prop('checked', false);
                    $("#add" + i + ",#user" + i + ",#pass" + i + ",#sel" + i + ",#save" + i).prop('disabled', true);
                    $("#on1h" + i + ",#on1m" + i + ",#off1h" + i + ",#off1m" + i + ",#on2h" + i + ",#on2m" + i + ",#off2h" + i + ",#off2m" + i + ",#os" + i + ",#or" + i).prop('disabled', false);
                } else {
                    $("#io" + i).prop('checked', true);
                    $("#add" + i + ",#user" + i + ",#pass" + i + ",#sel" + i + ",#save" + i).prop('disabled', false);
                    $("#on1h" + i + ",#on1m" + i + ",#off1h" + i + ",#off1m" + i + ",#on2h" + i + ",#on2m" + i + ",#off2h" + i + ",#off2m" + i + ",#os" + i + ",#or" + i).prop('disabled', true);
                }
                $("#out" + i).prop('checked', $xml.find("r in" + i).text() === "0" ? false : true);
                $("#de" + i).prop('checked', $xml.find("r is" + i).text() === "0" ? false : true);

                if ($("#io" + i).prop('checked')) {
                    $("#de" + i + ", #out" + i).prop('disabled', true);
                    $("#de" + i + ", #out" + i).next().find("span").addClass("onoff-disabled");
                } else {
                    $("#de" + i + ", #out" + i).prop('disabled', false);
                    $("#de" + i + ", #out" + i).next().find("span").removeClass("onoff-disabled");
                }

                // If output mode
                if ($xml.find("r io" + i).text() === "0") {
                    var on1h = $xml.find("r on1h" + i).text();
                    var on1m = $xml.find("r on1m" + i).text();
                    var off1h = $xml.find("r off1h" + i).text();
                    var off1m = $xml.find("r off1m" + i).text();
                    var on2h = $xml.find("r on2h" + i).text();
                    var on2m = $xml.find("r on2m" + i).text();
                    var off2h = $xml.find("r off2h" + i).text();
                    var off2m = $xml.find("r off2m" + i).text();
                    if (io) {
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
                        $("#out" + i).prop('disabled', false);
                        $("#out" + i).next().find("span").removeClass("onoff-disabled");
                    } else {
                        $("#out" + i).prop('disabled', true);
                        $("#out" + i).next().find("span").addClass("onoff-disabled");
                    }
                }
            }
        },
        error: function (jqXHR, textStatus, errorThrown) {
        },
        complete: function () {
            if (update) {
                setTimeout("updateIO(true, false);", 1500);
            }
        }
    });
}

function updateRemoteIO() {
    $.ajax({
        type: "GET",
        url: "/prt/io2.xml",
        cache: false,
        timeout: 6000,
        success: function (data) {
            $xml = $(data);
            for (var i = 1; i <= 8; i++) {
                $("#add" + i).val($xml.find("r ioAdd" + i).text());
                $("#user" + i).val($xml.find("r ioUser" + i).text());
                $("#pass" + i).val($xml.find("r ioPass" + i).text());
                $("#sel" + i).val($xml.find("r ioSel" + i).text());
            }

        },
        error: function (jqXHR, textStatus, errorThrown) {
        },
        complete: function () {
        }
    });
}

function enable(tag) {
    var id = tag.id;
    var num = id.substring(3, id.lenght);
    // TODO
    //var cond = $(tag).is(":checked") ? false : true;
    //$("#add" + num + ", #user" + num + ", #pass" + num).prop("disabled", cond);
}

function saveIO(tag) {
    var id = tag.id;
    var num = id.substring(4, id.lenght);

    showLoading(tag);
    var obj = {};
    obj['num'] = num;
    obj['add'] = $("#add" + num).val();
    obj['user'] = $("#user" + num).val();
    obj['pass'] = $("#pass" + num).val();
    obj['sel'] = $("#sel" + num).val();

    $.ajax({
        type: "POST",
        url: "/prt/io.htm",
        data: obj,
        cache: false,
        timeout: 20000,
        success: function (data) {
            showAccept(tag, 3000);
            //updateRemoteIO();
        },
        error: function (jqXHR, textStatus, errorThrown) {
            showExclamation(tag, 8000);
            showMessageInBox("#msg", "An error has occurred when updating.<br/>Error status: " + textStatus + "<br/>Error thrown: " + errorThrown, "fail", 10000);
        },
        complete: function () {
            hiddenLoading();
            updateRemoteIO();
        }
    });
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

function updateSelectWithOptionsIo() {
    for (var i = 1; i <= 8; i++) {
        for (var j = 0; j < 24; j++) {
            $('#on1h' + i + ',#off1h' + i + ',#on2h' + i + ',#off2h' + i).append($('<option>', {
                value: j,
                text: j
            }));
        }
        for (var j = 0; j < 60; j++) {
            $('#on1m' + i + ', #off1m' + i + ',#on2m' + i + ', #off2m' + i).append($('<option>', {
                value: j,
                text: j
            }));
        }
    }
}

function resetTimer(tag) {
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
    saveTimer(tag);
}

function saveTimer(tag) {

    var id = tag.id;
    var num = id.substring(2, id.lenght);

    showLoading(tag);
    $(tag).prop("disabled", true);

    var obj = {};
    obj['out'] = num;
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
            url: "/prt/io.cgi",
            data: obj,
            cache: false,
            timeout: 20000,
            success: function (data) {
                showAccept(tag, 3000);
                updateIO(false, true);
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
        showMessageInBox("#msg", "Verify Output " + num + " interval time and try again.", "fail", 10000);
    }
}
