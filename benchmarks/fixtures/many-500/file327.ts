interface Item327 { id: number; name: string; }
type Key327 = string | number;
export function make327(id: number, name: string): Item327 {
  const value: Item327 = {id, name};
  return value as Item327;
}
export const item327: Item327 = make327(327, "item-327");
