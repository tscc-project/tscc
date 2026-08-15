interface Item355 { id: number; name: string; }
type Key355 = string | number;
export function make355(id: number, name: string): Item355 {
  const value: Item355 = {id, name};
  return value as Item355;
}
export const item355: Item355 = make355(355, "item-355");
