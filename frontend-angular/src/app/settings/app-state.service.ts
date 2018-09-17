import { Injectable } from '@angular/core';

@Injectable()
export class AppState {

  public settings:any = undefined;
  public training: boolean = false;

  constructor() {}

}
