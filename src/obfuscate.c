int lss_obfuscate_value(int val)
{
	return val * 2 + 'l' + 'i' + 'v' + 'e';
}

int lss_unobfuscate_value(int val)
{
	return (val - 'l' - 'i' - 'v' - 'e') / 2;
}
