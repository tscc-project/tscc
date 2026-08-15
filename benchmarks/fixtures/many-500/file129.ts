interface Item129 { id: number; name: string; }
type Key129 = string | number;
export function make129(id: number, name: string): Item129 {
  const value: Item129 = {id, name};
  return value as Item129;
}
export const item129: Item129 = make129(129, "item-129");
