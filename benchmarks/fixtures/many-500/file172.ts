interface Item172 { id: number; name: string; }
type Key172 = string | number;
export function make172(id: number, name: string): Item172 {
  const value: Item172 = {id, name};
  return value as Item172;
}
export const item172: Item172 = make172(172, "item-172");
