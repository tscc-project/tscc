interface Item301 { id: number; name: string; }
type Key301 = string | number;
export function make301(id: number, name: string): Item301 {
  const value: Item301 = {id, name};
  return value as Item301;
}
export const item301: Item301 = make301(301, "item-301");
