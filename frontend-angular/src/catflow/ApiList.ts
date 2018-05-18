import {Observable} from "rxjs/Observable";
import {Injectable} from "@angular/core";
import {ApiRoute, ApiRouteContainig} from "./Utils";
import {BehaviorSubject} from "rxjs/BehaviorSubject";
import {ApiConnection} from "./ApiConnection";

/*
 * - list
 *    > subscribe to changes of list itself(add, remove) or list and members(add, update, remove)
 *    > perform action (add, get, update, remove)
 */
@Injectable()
export class ApiList<T> extends ApiRouteContainig
{
  private onChangeItems = new BehaviorSubject<T[]>([]);

  constructor(route: ApiRoute) {
    super(route);

    // get object
    ApiConnection.sendRequest(route, 'getAll', (status, data) => {
      if (status === 'ok')
        this.onChangeItems.next(data);
    });
  }

  /**
   * get the actual list of all items
   * @returns {BehaviorSubject<T[]>} update on list change (add, remove, update of object in list)
   */
  items(): BehaviorSubject<T[]> {
    return this.onChangeItems;
    /*
     * Observable not allows to get last set value
    return  new Observable<T[]>(observable => {
      this.onChangeItems.subscribe(value => observable.next(value));
    });
    */
  }

  /**
   * fires on changes
   * - item added:    type='add'
   * - item removed:  type='remove'
   * - item updated:  type='update'
   * @returns {Observable<{type: string; item?: T}>}
   */
  onChange(): Observable<{type: string, item?: T}> {
    return new Observable<{type: string, item?: T}>();
  }

  /**
   * adds item to list
   * @param item
   * @returns {Promise} add done
   */
  add(item: T): Promise<number>
  {
    return new Promise<number>((resolve, reject ) => {
      ApiConnection.sendRequest(this.route, "add", (status, data) => {
        if (status == 'ok')
        {
          // update items[]
          let lastItems: T[] = this.onChangeItems.getValue();
          if (lastItems == null)
            lastItems = [];

          lastItems.push(data);
          this.onChangeItems.next(lastItems);
          resolve(data.id);
        }
        else if (status == 'error')
          reject(data.message);
      }, item);
    });
  }

  /**
   * updates object with id=id
   * @param {number} id
   * @param {T} item values to update
   * @returns {Promise} update done
   */
  update(id: number, item: T): Promise<void> {
    return new Promise<void>((resolve, reject ) => {
      // test
      setTimeout(() => {
        reject('not implemented');
      }, 1000);
    });
  }

  /**
   * deletes old item at id and creates new at id
   * this means the item object is deleted and also all its children
   * @param {number} id
   * @param {T} item
   * @returns {Promise<void>}
   */
  replace(id: number, item: T): Promise<void> {
    return new Promise<void>((resolve, reject ) => {
      reject('replace of list is not implemented yes');
    });
  }

  /**
   * removes item at id
   * @param {number} id
   * @returns {Promise<void>}
   */
  remove(id: number): Promise<void> {
    return new Promise<void>(((resolve, reject) =>
    {
      ApiConnection.sendRequest(this.route +"/"+ id, "remove", (what, data) => {
        if (what === "ok")
        {
          // update items[]
          this.onChangeItems.next(this.getOnChangedItemsValue_withoutId(id));
          resolve();
        }
        else
          reject(data.message);
      })
    }));
  }

  /**
   * get items[] filtered without object with id=id
   * @param {number} id
   * @returns {T[]}
   */
  private getOnChangedItemsValue_withoutId(id: number): T[] {
    let arr =  this.onChangeItems.getValue();
    let result: T[] = [];
    for (let el of arr) {
      if (!('id' in el) || (el['id'] != id ))
        result.push(el);
    }
    return result;
  }
}