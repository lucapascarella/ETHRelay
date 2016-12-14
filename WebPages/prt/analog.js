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
        for (var j = 0; j < 1024; j++) {
            $('#ah' + i + ', #al' + i).append($('<option>', {
                value: j,
                text: j
            }));
        }
    }
}

function save(tag) {

    var id = tag.id;
    var num = id.substring(2, id.lenght);

    showLoading(tag);

    var obj = {};
    obj['adc'] = num;
    obj['ah'] = $("#ah" + num).val();
    obj['al'] = $("#al" + num).val();
    obj['ao'] = $("#ao" + num).val();
    obj['ar'] = $("#ar" + num).val();
    obj['ad'] = $("#ad" + num).val();

    $.ajax({
        type: "GET",
        url: "/prt/analog.cgi",
        data: obj,
        cache: false,
        timeout: 20000,
        success: function (data) {
            showAccept(tag, 3000);
            updateAnalog(false, true);
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

function updateAnalog(update, analog) {
    $.ajax({
        type: "GET",
        url: "/prt/analog.xml",
        cache: false,
        timeout: 6000,
        success: function (data) {
            $xml = $(data);
            for (var i = 1; i <= 4; i++) {
                var an = $xml.find("r a" + i).text();
                $("#ap" + i).val(an);
                $("#an" + i).text(an + "/1023   ");
                $("#v" + i).text((an * 5 / 1023).toFixed(2) + " V");
            }
            if (analog)
                for (var i = 1; i <= 4; i++) {
                    $("#ah" + i).val($xml.find("r h" + i).text());
                    $("#al" + i).val($xml.find("r l" + i).text());
                    $("#ao" + i).val($xml.find("r o" + i).text());
                    $("#ar" + i).val($xml.find("r r" + i).text());
                    $("#ad" + i).val($xml.find("r d" + i).text());
                }
        },
        error: function (jqXHR, textStatus, errorThrown) {
        },
        complete: function () {
            if (update)
                setTimeout("updateAnalog(true, false);", 700);
        }
    });
}
