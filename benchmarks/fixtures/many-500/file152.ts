interface Item152 { id: number; name: string; }
type Key152 = string | number;
export function make152(id: number, name: string): Item152 {
  const value: Item152 = {id, name};
  return value as Item152;
}
export const item152: Item152 = make152(152, "item-152");
