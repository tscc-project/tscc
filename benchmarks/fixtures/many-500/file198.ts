interface Item198 { id: number; name: string; }
type Key198 = string | number;
export function make198(id: number, name: string): Item198 {
  const value: Item198 = {id, name};
  return value as Item198;
}
export const item198: Item198 = make198(198, "item-198");
