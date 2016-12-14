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
            $('#onh' + i + ', #offh' + i).append($('<option>', {
                value: j,
                text: j
            }));
        }
    }
    for (var i = 1; i <= 4; i++) {
        for (var j = 0; j < 60; j++) {
            $('#onm' + i + ', #offm' + i).append($('<option>', {
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

function save(tag) {

    var id = tag.id;
    var num = id.substring(2, id.lenght);

    showLoading(tag);
    $(tag).prop("disabled", true);

    var obj = {};
    obj['relay'] = num;
    obj['onh'] = $("#onh" + num).val();
    obj['onm'] = $("#onm" + num).val();
    obj['offh'] = $("#offh" + num).val();
    obj['offm'] = $("#offm" + num).val();

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
            }

            if (relay) {
                for (var i = 1; i <= 4; i++) {
                    var onh = $xml.find("r onh" + i).text();
                    var onm = $xml.find("r onm" + i).text();
                    var offh = $xml.find("r offh" + i).text();
                    var offm = $xml.find("r offm" + i).text();
                    $("#onh" + i).val(onh);
                    $("#onm" + i).val(onm);
                    $("#offh" + i).val(offh);
                    $("#offm" + i).val(offm);
                }
            }
        },
        error: function (jqXHR, textStatus, errorThrown) {
        },
        complete: function () {
            $("#rs1, #rs2, #rs3, #rs4").prop("disabled", false);
            if (update) {
                setTimeout("updateRelay(true, false);", 1500);
            }
        }
    });
}

