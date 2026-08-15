interface Item154 { id: number; name: string; }
type Key154 = string | number;
export function make154(id: number, name: string): Item154 {
  const value: Item154 = {id, name};
  return value as Item154;
}
export const item154: Item154 = make154(154, "item-154");
