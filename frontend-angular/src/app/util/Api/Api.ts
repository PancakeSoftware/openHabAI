import {Injectable} from "@angular/core";
import { Operator } from 'rxjs/Operator';
import {Observable} from "rxjs/Observable";
import {reject} from "q";
import {ApiList} from "./ApiList";
import {ApiObject} from "./ApiObject";
import {ApiRoute} from "./Utils";

/*
 * with this class following api functions are available:
 * - list
 *    > subscribe to changes of list itself(add, remove) or list and members(add, update, remove)
 *    > perform action (add, get, update, remove)
 * - objects
 *    > subscribe to changes (add, update, remove)
 *    > perform action (add, get, update, remove)
 *
 * - send custom action to list, object
 *    > send [action: 'myAction', data: {...}]
 *    > get response [status: 'ok', data: {...}]
 */
@Injectable()
export class Api
{
  private listRoutes = new Map<string, any>();
  private objectRoutes = new Map<string, any>();

  constructor() {
    console.info('created Api!');
  }


  public list<T = any>(route: ApiRoute): ApiList<T> {
    if (this.listRoutes.has(route))
      return this.listRoutes.get(route);

    let list = new ApiList<T>(route);
    this.listRoutes.set(route, list);
    return list;
  }

  public object<T = any>(route: ApiRoute): ApiObject<T> {
    if (this.objectRoutes.has(route))
      return this.objectRoutes.get(route);

    console.info(route);

    let object = new ApiObject<T>(route);
    this.objectRoutes.set(route, object);
    return object;
  }
}