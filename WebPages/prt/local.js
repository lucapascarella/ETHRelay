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


var pingReq = false;
function askResult() {
    $.ajax({
        type: "GET",
        url: "/prt/local.xml",
        cache: false,
        timeout: 5000,
        success: function (data) {
            $xml = $(data);
            var pingIsComp = $xml.find("r pc").text();
            if (pingIsComp === "0") {
                $("#pingButton").prop("disabled", true);
                $("#urlButton").prop("disabled", true);
            }

            // Continue to ask ping status until the ping request is complete
            if (pingReq) {
                if (pingIsComp === "0") {
                    setTimeout("askResult()", 1500);
                    $("#res").text("Working...");
                } else {
                    pingReq = false;
                    if ($xml.find("r pt").text() === "1") {
                        $("#res").text("Request timed out");
                    } else if ($xml.find("r pu").text() === "1") {
                        $("#res").text('Can\'t resolve IP address');
                    } else {
                        $("#res").text("Resolved in: " + $xml.find("r tt").text() + " milliseconds, " + $xml.find("r ad").text());
                    }
                    hiddenLoading();
                    showAccept("#pingButton", 5000);
                }
            }
        },
        error: function (jqXHR, textStatus, errorThrown) {
            showExclamation(tag, 8000);
            showMessageInBox("#msg", "An error has occurred.<br/>Error status: " + textStatus + "<br/>Error thrown: " + errorThrown, "fail", 20000);
        },
        complete: function () {
        }
    });
}

function disPing(elem) {
    if (pingReq === false)
        $("#pingButton").prop("disabled", false);
}

function disUrl(elem) {
    if (pingReq === false)
        $("#urlButton").prop("disabled", false);
}

function pingIP(elem) {
    var val = $("#ping").val();
    if (checkIsIPV4(val)) {
        pingReq = true;
        showLoading(elem);
        //$.ajaxSetup({cache: false});
        //$.get("/prt/local.cgi?act=ping&res=" + val).always(setTimeout("askResult()", 700));

        var r = {};
        r['act'] = "ping";
        r['res'] = val;
        $.ajax({
            type: "GET",
            url: "/prt/local.cgi",
            data: r,
            cache: false,
            timeout: 10000,
            complete: function () {
                setTimeout("askResult()", 700);
            }
        });
        $(elem).prop("disabled", true);
    } else {
        alert("IP Address field is incorrect. Insert only a valid IPV4 address.");
    }
}

function urlIP(elem) {
    var val = $("#url").val();
    if (checkIsValidDomain(val)) {
        pingReq = true;
        showLoading(elem);
        var r = {};
        r['act'] = "url";
        r['res'] = val;
        $.ajax({
            type: "GET",
            url: "/prt/local.cgi",
            data: r,
            cache: false,
            timeout: 10000,
            complete: function () {
                setTimeout("askResult()", 700);
            }
        });
        $(elem).prop("disabled", true);
    } else {
        alert("IP Address field is incorrect. Insert only a valid IPV4 address.");
    }
}


function checkIsIPV4(entry) {
    var blocks = entry.split(".");
    if (blocks.length === 4) {
        return blocks.every(function (block) {
            return !isNaN(block) && parseInt(block, 10) >= 0 && parseInt(block, 10) <= 255;
        });
    }
    return false;
}

function checkIsValidDomain(domain) {
    var re = new RegExp(/^((?:(?:(?:\w[\.\-\+]?)*)\w)+)((?:(?:(?:\w[\.\-\+]?){0,62})\w)+)\.(\w{2,6})$/);
    return domain.match(re);
}

/*
 * Local network section
 */
function disableSaveLocal(cond) {
    $("#saveLocal").prop("disabled", cond);
//	$("#applyLocal").prop("disabled", cond);
    $("#saveAndApplyLocal").prop("disabled", cond);
    $("#resetValueLocal").prop("disabled", cond);
}

// Save parameters functions
function saveLocal(tag) {
    var obj = {};
    obj['save'] = "1";
    ajaxSaveLocal(obj, tag);
}

function saveAndApplyLocal(tag) {
    var obj = {};
    obj['save'] = "1";
    obj['rbt'] = "1";
    ajaxSaveLocal(obj, tag);
}

function ajaxSaveLocal(request, tag) {

    disableSaveLocal(true);
    showLoading(tag);
    request['mac'] = $("#mac").val();
    request['host'] = $("#host").val();
    request['icmp'] = $("#icmp").is(":checked") ? '1' : '0';
    request['dhcp'] = $("#dhcp").is(":checked") ? '1' : '0';
    request['ip'] = $("#ip").val();
    request['gw'] = $("#gw").val();
    request['sub'] = $("#sub").val();
    request['dns1'] = $("#dns1").val();
    request['dns2'] = $("#dns2").val();
    $.ajax({
        type: "POST",
        url: "/prt/local.htm",
        data: request,
        cache: false,
        timeout: 20000,
        success: function (data) {
            if (typeof request['appl'] !== "undefined") {
                // Asks for reboot.cgi that has a call to HTTPPrint_reboot
                $.ajaxSetup({cache: false});
                setTimeout("$.get('/prt/reboot.cgi');", 1500);
                $("#msg").html("The new settings are saved correctly.<br>The board will be restarted in 3 seconds.");
                setTimeout("$('#msg').html('The new settings are saved correctly.<br>The board will be restarted in 2 seconds.');", 1100);
                setTimeout("$('#msg').html('The new settings are saved correctly.<br>The board will be restarted in 1 second.');", 2100);
                setTimeout(function () {
                    console.log(data);
                    $("#msg").html("The new settings are saved correctly.<br>New location is: " + data);
                }, 3100);
                $("#msg").addClass("ok");
                $("#msg").show();
            } else {
                showMessageInBox("#msg", "The new settings are saved correctly but they will be available at the next restart", "ok", 15000);
            }
            showAccept(tag, 5000);
        },
        error: function (jqXHR, textStatus, errorThrown) {
            showExclamation(tag, 8000);
            showMessageInBox("#msg", "An error has occurred when updating.<br/>Error status: " + textStatus + "<br/>Error thrown: " + errorThrown, "fail", 20000);
        },
        complete: function () {
            hiddenLoading();
        }
    });
}

//function icmpCheck() {
//    if (role === 0x80) {
//        $("#icmp").prop("checked", !$("#icmp").is(":checked"));
//        dhcp();
//    }
//}

function icmp() {
    disableSaveLocal(false);
    var cond = false;
    if ($("#icmp").is(":checked"))
        cond = true;
    // Nothing to do
}

//function dhcpCheck() {
//    if (role === 0x80) {
//        $("#dhcp").prop("checked", !$("#dhcp").is(":checked"));
//        dhcp();
//    }
//}

function dhcp() {
    disableSaveLocal(false);
    var cond = false;
    if ($("#dhcp").is(":checked"))
        cond = true;
    $("#ip").prop("disabled", cond);
    $("#gw").prop("disabled", cond);
    $("#sub").prop("disabled", cond);
    $("#dns1").prop("disabled", cond);
    $("#dns2").prop("disabled", cond);
}

function resetValueLocal() {
    cond = false;
    $("#mac").prop('disabled', cond);
    $("#host").prop('disabled', cond);
    $("#icmp").prop('disabled', cond);
    $("#dhcp").prop('disabled', cond);
    $("#ip").prop('disabled', cond);
    $("#gw").prop('disabled', cond);
    $("#sub").prop('disabled', cond);
    $("#dns1").prop('disabled', cond);
    $("#dns2").prop('disabled', cond);
    $("#ping").prop("placeholder", gw_val);
    // Load values
    $("#mac").val(mac_val);
    $("#host").val(host_val);
    $("#hostLink").html("<a href='http://" + host_val.replace(/\s+$/, '') + "/' target='_blank'>http://" + host_val.replace(/\s+$/, '') + "/</a>");
    $("#icmp").prop("checked", icmp_val === "0" ? false : true);
    $("#dhcp").prop("checked", dhcp_val === "0" ? false : true);
    $("#ip").val(ip_val);
    $("#gw").val(gw_val);
    $("#sub").val(subnet_val);
    $("#dns1").val(dns1_val);
    $("#dns2").val(dns2_val);
    dhcp();
    // Disable Save buttons
    disableSaveLocal(true);
}



/*
 * DDNS section
 */
function disableSaveDNS(cond) {
    $("#saveDNS").prop("disabled", cond);
    $("#applyDNS").prop("disabled", cond);
    $("#saveAndApplyDNS").prop("disabled", cond);
    $("#resetValueDNS").prop("disabled", cond);
}

// Save parameters functions
function saveDNS(tag) {
    var obj = {};
    obj['save'] = "1";
    ajaxSaveDNS(obj, tag);
}

function saveAndApplyDNS(tag) {
    var obj = {};
    obj['save'] = "1";
    obj['appl'] = "1";
    ajaxSaveDNS(obj, tag);
}

function ajaxSaveDNS(request, tag) {

    disableSaveDNS(true);
    showLoading(tag);
    request['edns'] = $("#edns").is(":checked") === true ? "1" : "0";
    request['dsrv'] = $("#service").val();
    request['dhst'] = $("#dhost").val();
    request['dusr'] = $("#dusr").val();
    request['dpss'] = $("#dpss").val();
    $.ajax({
        type: "POST",
        url: "/prt/local.htm",
        data: request,
        cache: false,
        timeout: 20000,
        success: function (data) {
            if (typeof request['appl'] !== "undefined") {
                dns_enabled_val = request['edns'];
                showMessageInBox("#msg", "The new settings are saved and applied correctly", "ok", 15000);
            } else {
                showMessageInBox("#msg", "The new settings are saved correctly but they will be available at the next restart", "ok", 15000);
            }
            showAccept(tag, 5000);
        },
        error: function (jqXHR, textStatus, errorThrown) {
            showExclamation(tag, 8000);
            showMessageInBox("#msg", "An error has occurred when updating.<br/>Error status: " + textStatus + "<br/>Error thrown: " + errorThrown, "fail", 20000);
        },
        complete: function () {
            hiddenLoading();
        }
    });
}

function resetValueDNS() {
    cond = false;
    $("#service").prop('disabled', cond);
    $("#dhost").prop('disabled', cond);
    $("#dusr").prop('disabled', cond);
    $("#dpss").prop('disabled', cond);
    // Load values
    $("#public").text(dns_public_val);
    $("#edns").prop("checked", dns_enabled_val === "0" ? false : true);
    $("#service").val(dns_service_val);
    $("#dhost").val(dns_host_val);
    $("#dusr").val(dns_user_val);
    $("#dpss").val(dns_pass_val);
    edns();
    // Disable Save buttons
    disableSaveDNS(true);
}

//function ednsCheck() {
//    if (role === 0x80) {
//        $("#edns").prop("checked", !$("#edns").is(":checked"));
//        edns();
//    }
//}

function edns() {
    disableSaveDNS(false);
    if ($("#edns").is(":checked")) {
        cond = false;
    } else {
        cond = true;
    }
    $("#dshow").prop("disabled", cond);
    $("#service").prop("disabled", cond);
    $("#dhost").prop("disabled", cond);
    $("#dusr").prop("disabled", cond);
    $("#dpss").prop("disabled", cond);

}


//function showPassCheckDNS() {
//    if (role === 0x80 && $("#edns").is(":checked")) {
//        $("#dshow").prop("checked", !$("#dshow").is(":checked"));
//        showPasswordDNS();
//    }
//}

function showPasswordDNS() {
    if ($("#dshow").is(":checked"))
        $("#dpss").prop("type", "text");
    else
        $("#dpss").prop("type", "password");
}

function ddnsWorker() {

    $.ajax({
        type: "GET",
        url: '/prt/ddns.xml',
        cache: false,
        timeout: 6000,
        success: function (data) {
            $xml = $(data);
            // console.log($xml.find("r p").text());
            // console.log($xml.find("r s").text());
            $("#public").text($xml.find("r p").text());
            if (dns_enabled_val === '1') {
                switch ($xml.find("r s").text()) {
                    case '0':
                        $("#dnss").text("Update successful, hostname is now updated");
                        break;
                    case '1':
                        $("#dnss").text("Update changed no setting and is considered abusive.  Additional 'nochg' updates will cause hostname to be blocked.");
                        break;
                    case '2':
                        $("#dnss").text("The hostname specified is blocked for update abuse.");
                        break;
                    case '3':
                        $("#dnss").text("System parameter not valid. Should be dyndns, statdns or custom.");
                        break;
                    case '4':
                        $("#dnss").text("The user agent was blocked or not sent.");
                        break;
                    case '5':
                        $("#dnss").text("The username and password pair do not match a real user.");
                        break;
                    case '6':
                        $("#dnss").text("An option available only to credited users (such as offline URL) was specified, but the user is not a credited user. If multiple hosts were specified, only a single !donator will be returned.");
                        break;
                    case '7':
                        $("#dnss").text("The hostname specified is not a fully-qualified domain name (not in the form hostname.dyndns.org or domain.com).");
                        break;
                    case '8':
                        $("#dnss").text("The hostname specified does not exist in this user account (or is not in the service specified in the system parameter).");
                        break;
                    case '9':
                        $("#dnss").text("The hostname specified does not belong to this user account.");
                        break;
                    case '10':
                        $("#dnss").text("Too many hosts specified in an update.");
                        break;
                    case '11':
                        $("#dnss").text("Unspecified DNS error encountered by the DDNS service.");
                        break;
                    case '12':
                        $("#dnss").text("There is a problem or scheduled maintenance with the DDNS service.");
                        break;
                    case '13':
                        $("#dnss").text("Error communicating with Update service.");
                        break;
                    case '14':
                        $("#dnss").text("The IP Check indicated that no update was necessary.");
                        break;
                    case '15':
                        $("#dnss").text("Error communicating with CheckIP service.");
                        break;
                    case '16':
                        $("#dnss").text("DDNS Client data is not valid.");
                        break;
                    case '17':
                        $("#dnss").text("DDNS client has not yet been executed with this configuration.");
                        break;
                }
            } else {
                $("#dnss").text("DDNS Service disabled");
            }
        },
        error: function (jqXHR, textStatus, errorThrown) {
            $("#dnss").text("Connection lost");
        },
        complete: function () {
            setTimeout("ddnsWorker();", 1600);
        }
    });
}