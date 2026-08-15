interface Item291 { id: number; name: string; }
type Key291 = string | number;
export function make291(id: number, name: string): Item291 {
  const value: Item291 = {id, name};
  return value as Item291;
}
export const item291: Item291 = make291(291, "item-291");
