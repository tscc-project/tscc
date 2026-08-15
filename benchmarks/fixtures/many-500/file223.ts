interface Item223 { id: number; name: string; }
type Key223 = string | number;
export function make223(id: number, name: string): Item223 {
  const value: Item223 = {id, name};
  return value as Item223;
}
export const item223: Item223 = make223(223, "item-223");
