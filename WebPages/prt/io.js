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
                $("#io" + i).prop('checked', $xml.find("r io" + i).text() === "0" ? false : true);
                $("#out" + i).prop('checked', $xml.find("r in" + i).text() === "0" ? false : true);
                $("#de" + i).prop('checked', $xml.find("r is" + i).text() === "0" ? false : true);

                if ($("#io" + i).prop('checked')) {
                    $("#de" + i + ", #out" + i).prop('disabled', true);
                    $("#de" + i + ", #out" + i).next().find("span").addClass("onoff-disabled");
                } else {
                    $("#de" + i + ", #out" + i).prop('disabled', false);
                    $("#de" + i + ", #out" + i).next().find("span").removeClass("onoff-disabled");
                }
            }

        },
        error: function (jqXHR, textStatus, errorThrown) {
        },
        complete: function () {
            if (update)
                setTimeout("updateIO(true, false);", 1500);
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
