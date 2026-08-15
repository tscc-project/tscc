interface Item346 { id: number; name: string; }
type Key346 = string | number;
export function make346(id: number, name: string): Item346 {
  const value: Item346 = {id, name};
  return value as Item346;
}
export const item346: Item346 = make346(346, "item-346");
