interface Item422 { id: number; name: string; }
type Key422 = string | number;
export function make422(id: number, name: string): Item422 {
  const value: Item422 = {id, name};
  return value as Item422;
}
export const item422: Item422 = make422(422, "item-422");
