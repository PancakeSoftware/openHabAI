import {Subject} from "rxjs/src/Subject";
import {Subscriber} from "rxjs/src/Subscriber";
import {ISubscription, Subscription} from "rxjs/src/Subscription";
import {ObjectUnsubscribedError} from "rxjs/src/util/ObjectUnsubscribedError";

/**
 * @class PublishSubject<T>
 */
export class PublishSubject<T> extends Subject<T> {
  private _value: T = undefined;

  constructor() {
    super();
  }

  get value(): T {
    return this.getValue();
  }

  /** @deprecated internal use only */ _subscribe(subscriber: Subscriber<T>): Subscription {
    const subscription = super._subscribe(subscriber);
    if (subscription && !(<ISubscription>subscription).closed) {
      if (this._value !== undefined)
        subscriber.next(this._value);
    }
    return subscription;
  }

  getValue(): T {
    if (this.hasError) {
      throw this.thrownError;
    } else if (this.closed) {
      throw new ObjectUnsubscribedError();
    } else {
      return this._value;
    }
  }

  next(value: T): void {
    super.next(this._value = value);
  }
}
