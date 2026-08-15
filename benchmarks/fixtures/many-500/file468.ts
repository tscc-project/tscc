interface Item468 { id: number; name: string; }
type Key468 = string | number;
export function make468(id: number, name: string): Item468 {
  const value: Item468 = {id, name};
  return value as Item468;
}
export const item468: Item468 = make468(468, "item-468");
