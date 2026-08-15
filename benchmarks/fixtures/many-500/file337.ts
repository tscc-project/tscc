interface Item337 { id: number; name: string; }
type Key337 = string | number;
export function make337(id: number, name: string): Item337 {
  const value: Item337 = {id, name};
  return value as Item337;
}
export const item337: Item337 = make337(337, "item-337");
