interface Item309 { id: number; name: string; }
type Key309 = string | number;
export function make309(id: number, name: string): Item309 {
  const value: Item309 = {id, name};
  return value as Item309;
}
export const item309: Item309 = make309(309, "item-309");
