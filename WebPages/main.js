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
function doLogout() {

    var url = document.URL;
    var address = url.substring(url.indexOf("//") + 2);
    var fakeUrl = "http://logout:logout@" + document.location.hostname + "/prt/index.htm";

    document.execCommand("ClearAuthenticationCache", "false");

    $.ajax({
        type: "GET",
        url: fakeUrl,
        cache: false,
        timeout: 20000,
        success: function (data) {
            // Do nothing
        },
        error: function (jqXHR, textStatus, errorThrown) {
            // Do nothing
        },
        complete: function () {
            doLogoutRedirect();
        }
    });
}

function doLogoutRedirect(x) {
    var fakeUrl = "http://" + document.location.hostname + "/index.htm";
    window.location.href = fakeUrl;
}

function updateStatus() {
    $.ajax({
        type: "GET",
        url: "/cs.xml",
        cache: false,
        timeout: 6000,
        success: function (data) {
            $xml = $(data);
            var cs = $xml.find("cs").text();
            $("#status").removeClass("red");
            var str;
            switch (cs) {
                case "0": //  0 = Connected
                    str = "Connected";
                    break;
                case "1": //  1 = BT Broadcaster Transmitter UDP
                    str = "To be defined";
                    break;
                default:
                    str = "Undefined";
                    $("#status").addClass("red");
            }
            $("#status").text(str);
        },
        error: function (jqXHR, textStatus, errorThrown) {
            $("#status").addClass("red");
            $("#status").text("Connection lost");
        },
        complete: function () {
            setTimeout("updateStatus();", 5000);
        }
    });
}

/*
 * This function select the appropriate button in the menu
 */
function selectMenu(item) {
    $("#menu a").removeClass("sel");
    $("#menu a:nth-child(" + item + ")").addClass("sel");
}

/*
 * The following functions append and remove the spinner, accept and exclamation images during AJAX transaction
 */
function showLoading(tag) {
    $(tag).after('<img class="hidden loading" src="../imgs/spinner.gif" alt="Loading" title="Loading in progress...">');
    $(".loading").show();
}

function hiddenLoading() {
    $(".loading").hide("slow");
    setTimeout("$('.loading').remove();", 1000);
}

function showAccept(tag, time) {
    $(tag).after('<img class="hidden accept" src="../imgs/accept.png" alt="Accept" title="Request accepted successful"">');
    $(".accept").show();
    setTimeout("$('.accept').hide('slow');", time);
    setTimeout("$('.accept').remove();", (time + 1000));
}

function showExclamation(tag, time) {
    $(tag).after('<img class="hidden exclamation" src="../imgs/exclamation.png" alt="Error" title="An error has occurred">');
    $(".exclamation").show();
    setTimeout("$('.exclamation').hide('slow');", time);
    setTimeout("$('.exclamation').remove();", (time + 1000));
}

/*
 * 
 */
function showMessageInBox(tagId, message, classId, timeout) {
    $(tagId).show("fast");
    $(tagId).removeClass("ok unk fail");
    $(tagId).addClass(classId);
    $(tagId).html(message);
    if (timeout !== 0)
        setTimeout("$('" + tagId + "').hide('slow');", timeout);
}
