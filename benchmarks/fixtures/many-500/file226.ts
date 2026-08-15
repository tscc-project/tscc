interface Item226 { id: number; name: string; }
type Key226 = string | number;
export function make226(id: number, name: string): Item226 {
  const value: Item226 = {id, name};
  return value as Item226;
}
export const item226: Item226 = make226(226, "item-226");
