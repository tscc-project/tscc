interface Item136 { id: number; name: string; }
type Key136 = string | number;
export function make136(id: number, name: string): Item136 {
  const value: Item136 = {id, name};
  return value as Item136;
}
export const item136: Item136 = make136(136, "item-136");
