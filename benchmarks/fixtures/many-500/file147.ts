interface Item147 { id: number; name: string; }
type Key147 = string | number;
export function make147(id: number, name: string): Item147 {
  const value: Item147 = {id, name};
  return value as Item147;
}
export const item147: Item147 = make147(147, "item-147");
