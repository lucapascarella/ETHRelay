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

//function showPassCheck() {
//    $("#show").prop('checked', !$("#show").is(':checked'));
//    showPassword();
//}

function showPassword() {
    if ($("#show").is(":checked"))
        $("#pass").prop("type", "text");
    else
        $("#pass").prop("type", "password");
}

function disableSaveAuth(cond) {
    $("#saveAuth").prop("disabled", cond);
    $("#saveAndApplyAuth").prop("disabled", cond);
}

// Save parameters functions
function saveAuth(tag) {
    var obj = {};
    obj['save'] = "1";
    ajaxSaveAuth(obj, tag);
}

function saveAndApplyAuth(tag) {
    var obj = {};
    obj['save'] = "1";
    obj['appl'] = "1";
    ajaxSaveAuth(obj, tag);
}

function ajaxSaveAuth(request, tag) {

    showLoading(tag);
    disableSaveAuth(true);

    request['user'] = $("#user").val();
    request['pass'] = $("#pass").val();

    $.ajax({
        type: "POST",
        url: "/prt/index.htm",
        data: request,
        cache: false,
        timeout: 20000,
        success: function (data) {
            showAccept(tag, 5000);
            if (request['appl'] === "1") {
                $('#waitLogout').show("slow");
                setTimeout("doLogout();", 2000);
            } else {
                $("#passwordChanged").show("slow");
            }
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


