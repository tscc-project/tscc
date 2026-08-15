interface Item406 { id: number; name: string; }
type Key406 = string | number;
export function make406(id: number, name: string): Item406 {
  const value: Item406 = {id, name};
  return value as Item406;
}
export const item406: Item406 = make406(406, "item-406");
