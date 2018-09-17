import {Component, OnInit} from '@angular/core';
import {Api} from "@catflow/Api";
import {ApiObject} from "@catflow/ApiObject";
import {toastOk} from "@frontend/util/Log";
import {AppState} from "@frontend/settings/app-state.service";

@Component({
    selector: 'app-settings',
    templateUrl: './settings.component.html',
    styles: []
})
export class SettingsComponent implements OnInit {

    settings: any = undefined;
    settingsKeys: any[] = [];

    constructor(private api: Api, settingsServ:AppState) {
        api.object("/settings/").subscribe();
        api.object("/settings/").object().subscribe(value => {
            if (value == null)
                return;

            this.settings = value;
            this.settingsKeys = Object.keys(this.settings);
            console.info("object changed!  ", this.settings)
            settingsServ.settings = this.settings;
        });
    }

    update(setting: string, value: any) {
        let a: any = {};
        if (!this.isNumber(setting))
            a[setting] = value;
        else
            a[setting] = Number(value);
        this.api.object("/settings/").update(a);
    }

    isNumber(setting: string)
    {
        return typeof this.settings[setting] === "number";
    }

    isObject(setting: string)
    {
        return typeof this.settings[setting] === "object"
    }

    onTriggerStopServer() {
        this.api.object("/settings/").action("stopServer")
            .then( () => {
                toastOk("Stopped Server");
        });
    }

    ngOnInit() {
    }

}