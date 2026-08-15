interface Item238 { id: number; name: string; }
type Key238 = string | number;
export function make238(id: number, name: string): Item238 {
  const value: Item238 = {id, name};
  return value as Item238;
}
export const item238: Item238 = make238(238, "item-238");
