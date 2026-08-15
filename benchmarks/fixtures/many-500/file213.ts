interface Item213 { id: number; name: string; }
type Key213 = string | number;
export function make213(id: number, name: string): Item213 {
  const value: Item213 = {id, name};
  return value as Item213;
}
export const item213: Item213 = make213(213, "item-213");
